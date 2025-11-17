#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "System.h"
#include <glm/glm.hpp>

namespace gea
{

class CameraSystem : public System
{
private:
    glm::mat4x4 mProjectionMatrix{glm::mat4(1.0f)}; //How things will look in the viewport
    glm::mat4x4 mViewMatrix{glm::mat4(1.0f)};       //Actual position of camera

public:

    CameraSystem() {};
    ~CameraSystem() {};
    void Update();
    void InitializeCamera();
    glm::mat4x4 Perspective(double degrees, double aspect, double nearplane, double farplane);
    glm::mat4x4 LookAt(const glm::vec3 &Position, const glm::vec3 &Direction, const glm::vec3 &Up);


};

} // End of namespace gea

#endif // CAMERASYSTEM_H
