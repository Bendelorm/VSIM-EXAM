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

    // NEW:
    static void setEnabled(bool enabled);
    static bool isEnabled() { return s_enabled; }

private:
    static void resetAllPhysics();   // reset velocities etc.

    static bool s_enabled;
};
}
