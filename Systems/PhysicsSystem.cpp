#include "PhysicsSystem.h"
#include "../engineInit.h"       // case-correct include
#include "../TransformManager.h" // if you need it

namespace gea
{
void PhysicsSystem::update(float dt, Renderer* renderer)
{
    if (!renderer)
        return;

    // 1) Find the terrain
    EntityRenderData* terrainEntity = renderer->getFirstTerrainEntity();
    if (!terrainEntity)
        return;

    EntityRenderData& terrain = *terrainEntity;

    // 2) Loop over physics components
    for (auto& [entityID, phys] : EngineInit::registry.Physics)
    {
        auto transformIt = EngineInit::registry.Transforms.find(entityID);
        if (transformIt == EngineInit::registry.Transforms.end())
            continue;

        gea::Transform& transform = transformIt->second;
        glm::vec3& pos = transform.mPosition;

        // 3) Terrain intersection
        TerrainHit hit = renderer->findTriangleUnderBall(terrain, pos);
        glm::vec3 a(0.0f);

        if (hit.hit)
        {
            float terrainY = hit.height + phys.radius;
            float verticalDist = pos.y - hit.height;

            // Is the ball close enough to be considered grounded?
            bool onGround = (pos.y <= terrainY + 0.001f);

            glm::vec3 g(0.0f, -9.81f, 0.0f);

            if (onGround)
            {
                // Project gravity onto the slope
                float gDotN = glm::dot(g, hit.normal);
                a = g - gDotN * hit.normal;

                // Prevent sinking
                if (pos.y < terrainY)
                    pos.y = terrainY;

                // Remove downward velocity
                if (phys.velocity.y < 0.0f)
                    phys.velocity.y = 0.0f;
            }
            else
            {
                // Free fall
                a = g;
            }
        }
        else
        {
            // No terrain below → free fall
            a = glm::vec3(0.0f, -9.81f, 0.0f);
        }

        phys.acceleration = a;

        // Euler integration
        phys.velocity += phys.acceleration * dt;
        pos          += phys.velocity * dt;
    }
}
}
