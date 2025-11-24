#include "PhysicsSystem.h"
#include "../engineInit.h"
#include "../TransformManager.h"

#include <glm/gtc/quaternion.hpp>

bool gea::PhysicsSystem::s_enabled = false;

namespace gea
{
static bool isInsideFrictionZone(const glm::vec3& p)
{
    //tweak these to match the area I want
    const float minX =  -10.0f;
    const float maxX =  40.0f;
    const float minZ =  5.0f;
    const float maxZ =  40.0f;

    return (p.x >= minX && p.x <= maxX &&
            p.z >= minZ && p.z <= maxZ);
}

void PhysicsSystem::setEnabled(bool enabled)
{
    s_enabled = enabled;

    // When turning physics OFF, reset all physics components
    if (!s_enabled)
        resetAllPhysics();
}

void PhysicsSystem::resetAllPhysics()
{
    for (auto& [entityID, phys] : EngineInit::registry.Physics)
    {
        phys.velocity        = glm::vec3(0.0f);
        phys.acceleration    = glm::vec3(0.0f);
        phys.currentTriIndex = -1;
        phys.orientation     = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
}

void PhysicsSystem::update(float dt, Renderer* renderer)
{
    if (!s_enabled) return;
    if (!renderer) return;

    EntityRenderData* terrainEntity = renderer->getFirstTerrainEntity();
    if (!terrainEntity) return;
    EntityRenderData& terrain = *terrainEntity;

    const float g = 9.81f;
    const glm::vec3 gvec(0.0f, -g, 0.0f);


    //AABB Obstacle (Default blender cube is 2x2x2 so half is 1)
    glm::vec3 obstacleMin(0.0f);
    glm::vec3 obstacleMax(0.0f);
    bool hasObstacle = false;

    for (auto& [id, tr] : EngineInit::registry.Transforms)
    {
        if (tr.isObstacle)
        {
            glm::vec3 halfExtents = 1.0f * tr.mScale;
            glm::vec3 center      = tr.mPosition;

            obstacleMin = center - halfExtents;
            obstacleMax = center + halfExtents;

            hasObstacle = true;
            break;
        }
    }

    // Loop all entities with Physics
    for (auto& [entityID, phys] : EngineInit::registry.Physics)
    {
        auto trIt = EngineInit::registry.Transforms.find(entityID);
        if (trIt == EngineInit::registry.Transforms.end())
            continue;

        gea::Transform& transform = trIt->second;
        glm::vec3& pos = transform.mPosition;

        // 1) Find which triangle the ball is on (Algorithm 9.6, step 1)
        TerrainHit hit = renderer->findTriangleUnderBallWithHint(
            terrain, pos, phys.currentTriIndex);

        glm::vec3 a(0.0f);      // acceleration
        bool onSurface = false; // "in contact" this frame

        if (hit.hit)
        {
            glm::vec3 n = glm::normalize(hit.normal);
            phys.currentTriIndex = hit.triIndex;

            float surfaceY = hit.height + phys.radius;
            float distToSurface = pos.y - surfaceY;

            // Contact if we're at or below the surface and moving into it
            bool penetrating = (distToSurface < 0.0f);
            bool touching    = std::abs(distToSurface) <= 0.002f;

            if (penetrating)
            {
                // push up and kill downward normal velocity
                pos.y = surfaceY;
                float vDotN = glm::dot(phys.velocity, n);
                if (vDotN < 0.0f)
                    phys.velocity -= vDotN * n;   // remove only inward component
                onSurface = true;
            }
            else if (touching && glm::dot(phys.velocity, n) <= 0.0f)
            {
                //resting on surface
                onSurface = true;
            }

            if (onSurface)
            {
                // Acceleration along slope (eq. 9.14)
                float gDotN = glm::dot(gvec, n);
                a = gvec - gDotN * n;   // base slope acceleration

                // BASE FRICTION EVERYWHERE ON SURFACE
                glm::vec3 vTan = phys.velocity - glm::dot(phys.velocity, n) * n;
                float speed = glm::length(vTan);

                if (speed > 0.001f)
                {
                    glm::vec3 dir = vTan / speed;  // unit tangent

                    // baseline friction coefficient (applied everywhere)
                    const float muBase  = 0.1f;   // tweak as you like
                    float mu = muBase;

                    // EXTRA FRICTION IN SPECIAL ZONE
                    if (isInsideFrictionZone(pos))
                    {
                        const float muExtra = 0.2f; // additional friction in red area
                        mu += muExtra;
                    }
                    glm::vec3 aFriction = -mu * g * dir;

                    // total acceleration = slope acceleration + friction
                    a += aFriction;
                }
            }

            else
            {
                // free fall
                a = gvec;
            }
        }
        else
        {
            // No terrain under us -> free fall
            a = gvec;
            phys.currentTriIndex = -1;
            onSurface = false;
        }

        phys.acceleration = a;

        //Update velocity (eq. 9.16)
        glm::vec3 v_k = phys.velocity;
        phys.velocity = v_k + phys.acceleration * dt;

        //Update position (eq. 9.17)
        pos = pos + v_k * dt + 0.5f * phys.acceleration * (dt * dt);

        // If we ended up under the surface due to integration, push up
        if (hit.hit)
        {
            glm::vec3 n = glm::normalize(hit.normal);
            float surfaceY = hit.height + phys.radius;
            if (pos.y < surfaceY)
            {
                pos.y = surfaceY;
                float vDotN = glm::dot(phys.velocity, n);
                if (vDotN < 0.0f)
                    phys.velocity -= vDotN * n;
                onSurface = true;
            }
        }
        //Collision with obstacle (sphere vs AABB)
        if (hasObstacle)
        {
            // Closest point on the box to the ball center
            glm::vec3 closest = glm::clamp(pos, obstacleMin, obstacleMax);
            glm::vec3 diff    = pos - closest;
            float dist2       = glm::dot(diff, diff);

            float r = phys.radius;
            if (dist2 < r * r)
            {
                float dist = std::sqrt(dist2);

                glm::vec3 n;
                if (dist > 0.001f)
                {
                    n = diff / dist;   // collision normal
                }
                else
                {
                    // Ball center is exactly inside box center: choose axis
                    glm::vec3 boxCenter = 0.5f * (obstacleMin + obstacleMax);
                    glm::vec3 local = pos - boxCenter;
                    glm::vec3 absLocal = glm::abs(local);

                    if (absLocal.x > absLocal.y && absLocal.x > absLocal.z)
                        n = glm::vec3((local.x > 0.0f) ? 1.0f : -1.0f, 0.0f, 0.0f);
                    else if (absLocal.y > absLocal.z)
                        n = glm::vec3(0.0f, (local.y > 0.0f) ? 1.0f : -1.0f, 0.0f);
                    else
                        n = glm::vec3(0.0f, 0.0f, (local.z > 0.0f) ? 1.0f : -1.0f);
                }

                float penetration = r - (dist > 0.001f ? dist : 0.0f);

                // Push ball out of the obstacle
                pos += n * penetration;

                // Remove velocity into the obstacle
                float vDotN = glm::dot(phys.velocity, n);
                if (vDotN < 0.0f)
                    phys.velocity -= vDotN * n;
            }
        }

        //Rolling rotation: only while in contact (Not working properly)
        if (hit.hit && onSurface)
        {
            glm::vec3 n = glm::normalize(hit.normal);

            // Tangential velocity
            glm::vec3 vTan =
                phys.velocity - glm::dot(phys.velocity, n) * n;

            float speed = glm::length(vTan);
            if (speed > 0.001f)
            {
                // Axis of rotation
                glm::vec3 axis = glm::normalize(glm::cross(n, vTan));

                // No-slip rolling
                float omega = speed / phys.radius;   // rad/s
                float angle = omega * dt;            // this frame

                glm::quat dq = glm::angleAxis(angle, axis);
                phys.orientation = glm::normalize(dq * phys.orientation);

                glm::vec3 eulerRad = glm::eulerAngles(phys.orientation);
                transform.mRotation = glm::degrees(eulerRad);
            }
        }
    }
}

} // namespace gea
