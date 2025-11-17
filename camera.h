#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>
class Camera
{
public:
    Camera();
    ~Camera() {};


    void initializeCamera();
    void perspective(int degrees, double aspect, double nearplane, double farplane);
    void lookAt(const glm::vec3 &eye, const glm::vec3 &at, const glm::vec3 &up);
    void setSpeed (float speed);
    void move();

    //setter
    inline void setViewMatrix(const glm::mat4x4 &newViewMatrix){ mViewMatrix = newViewMatrix; }
    inline void setProjectionMatrix(const glm::mat4x4 &newProjectionMatrix){ mProjectionMatrix = newProjectionMatrix; }

    //getter
    inline glm::mat4x4  viewMatrix() const { return mViewMatrix; }
    inline glm::mat4x4  projectionMatrix() const { return mProjectionMatrix; }

    ///
    /// @brief Moves camera to a given position
    /// @param position
    ///
    void translate(glm::vec3 position ){ setViewMatrix(glm::translate(mViewMatrix, position));}
    ///
    /// @brief Rotate the camera
    /// @param angle
    /// @param axis  What axis to rotate on
    ///
    void rotate(float angle, glm::vec3 axis){setViewMatrix(glm::rotate(mViewMatrix,angle,axis));}

private:
    glm::vec3 mEye{0.0f,0.0f,0.0f};  //Camera position
    glm::vec3 mAt {0.0f,0.0f,-1.0f}; //Forward vector
    glm::vec3 mUp {0.0f,1.0f,0.0f};  //Up vector


    glm::mat4x4 mProjectionMatrix{}; //How things will look in the viewport
    glm::mat4x4 mViewMatrix{};       //Actual position of camera

    glm::vec3 mPosition{0.f,0.f,0.f};
    float mPitch{0.f};
    float mYaw{0.f};

    float mSpeed{1.f};
};

#endif // CAMERA_H
