#pragma once

#include <glm/glm.hpp>
#include "../Renderer.h"     // IMPORTANT: gives you Renderer, EntityRenderData, TerrainHit
#include "../Components/Components.h"

namespace gea
{
class PhysicsSystem
{
public:
    static void update(float dt, Renderer* renderer);
};
}
