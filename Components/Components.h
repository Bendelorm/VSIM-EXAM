#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

//Need namespace, since we start to get naming collisions with other code
namespace gea
{

//Add new component types here
enum class ComponentTypes
{
    Transform,
    Mesh,
    Texture,
    Camera
};


//Below are examples of actual components
//Systems need each component to know what entity it belongs to

struct Component
{
    short mEntityID{-1};
    std::string path;
};

struct Transform : Component
{
    std::string name;
    glm::vec3 mPosition{0.0f, 0.0f, 0.0f};
    glm::vec3 mRotation{0.0f, 0.0f, 0.0f};
    glm::vec3 mScale{1.0f, 1.0f, 1.0f};
};

struct Mesh : Component
{
    short mMesh{-1};        //index into array of all meshes loaded
};

struct Texture : Component
{
    short mTexture{-1};        //index into array of all textures loaded
    std::string path;
};

struct Camera: Component
{
    glm::mat4x4 mProjectionMatrix{}; //How things will look in the viewport
    glm::mat4x4 mViewMatrix{};       //Actual position of camera

    //NOTE: Should this part be its own component? Like a seperate transform for camera
    glm::vec3 Position{-10.0f,1.0f,0.0f};  //Camera position
    glm::vec3 Direction {0.0f,0.0f,-1.0f}; //Forward vector
    glm::vec3 Right{1.0f,0.0f,0.0f};
    glm::vec3 Up {0.0f,1.0f,0.0f};  //Up vector


    float mSpeed{0.01f};          //Camera will move at this speed
    float mRotationSpeed{1.0f}; // Camera will rotate at this speed
    float mPitch{0.5f};
    float mYaw{.5f};
    float mRoll{0.f};

    //Lens
    //NOTE: Own camera lens component?
    double mFieldOfView{60.f}; //In degrees
    double mAspect{1.57142857f}; //TODO: must be a better way to get the aspect ratio
    double mNearplane{0.1f};
    double farplane {100.0f};

    bool isActive{false};

};
//TODO: SoundSource - Expand sound component
// struct SoundSource
// {

// };


//The plan is that the systems can use these vectors containing all components of the different types
//The components should be sorted by EntityID when added to the vectors.
//A Render system then might need many vectors, but a Move system might only need one or two.
//Each system just access the ones they need, and iterates through them and do their job.
//Since the components are sorted by EntityID, the iteration should go well and we should get DOD benefits.

// Vector removed, check registry.h

}   //namespace gea

#endif // COMPONENTS_H
