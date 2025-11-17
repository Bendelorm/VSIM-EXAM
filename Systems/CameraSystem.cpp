#include "CameraSystem.h"
#include "../engineinit.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "../Input.h"

namespace gea
{

/// @brief updates the camera based on current conditions
void CameraSystem::Update()
{
    //Get the entity with an active camera
    //do the updating
    //simple

    //NOTE: Goal is to have an editor- and play mode version. For now, controls one camera

    for(auto &[entityID,component] : gea::EngineInit::registry.Cameras)
    {

        //quick math for rotation from https://learnopengl.com/Getting-started/Camera
        component.Direction.x = glm::cos(glm::radians(component.mYaw)) * glm::cos(glm::radians(component.mPitch ));
        component.Direction.y = glm::sin(glm::radians(component.mPitch));
        component.Direction.z = glm::sin(glm::radians(component.mYaw)) * cos(glm::radians(component.mPitch));
        component.mViewMatrix = LookAt(component.Position, component.Position + component.Direction, component.Up);


        //Calculate right vector
        component.Right = glm::normalize(glm::cross(component.Direction, component.Up));

        component.mProjectionMatrix = Perspective(component.mFieldOfView,component.mAspect,component.mNearplane,component.farplane);

    }
}

/// @brief Create perspective matrix
/// @param degrees Expressend in radians
/// @param aspect
/// @param nearplane
/// @param farplane
glm::mat4x4 CameraSystem::Perspective(double degrees, double aspect, double nearplane, double farplane)
{
    return glm::perspective(glm::radians(degrees),aspect,nearplane,farplane);
}

/// @brief Constructs the viewmatrix
/// @param eye Position of camera
/// @param at What the camera is looking at
/// @param up Which way is up
/// @return 4x4 matrix
glm::mat4x4 CameraSystem::LookAt(const glm::vec3 &Position, const glm::vec3 &Direction, const glm::vec3 &up)
{
    return glm::lookAt(Position,Direction,up);
}

} //End of namespace gea
