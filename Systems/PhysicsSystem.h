#pragma once

#include <glm/glm.hpp>
#include "../Renderer.h"
#include "../Components/Components.h"

namespace gea
{
class PhysicsSystem
{
public:
    static void update(float dt, Renderer* renderer);


    static void setEnabled(bool enabled);
    static bool isEnabled() { return s_enabled; }
    static void clearTrace();

private:
    static void resetAllPhysics();   // reset velocities etc.

    // B-spline helpers
    static void addTraceSample(const glm::vec3& p);
    static void rebuildTraceCurve(Renderer* renderer);
    static glm::vec3 evalCubicUniformBSplineSegment(
        const glm::vec3& P0,
        const glm::vec3& P1,
        const glm::vec3& P2,
        const glm::vec3& P3,
        float t);

    static bool s_enabled;
    static float s_simTime;

    //trace state
    static float s_traceTimer;
    static float s_traceSampleInterval;
    static std::vector<glm::vec3> s_traceControlPoints; // control points (ball positions)
};
}
