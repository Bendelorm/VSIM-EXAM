
#ifndef REGISTRY_H
#define REGISTRY_H

#include <unordered_map>   // defines std::unordered_map
#include "Components/Components.h"
#include "input.h"
class registry
{
public:
    registry() {};

    std::unordered_map<short, gea::Camera> Cameras;
    std::unordered_map<short, gea::Transform> Transforms;
    std::unordered_map<short, gea::Mesh> Meshes;
    std::unordered_map<short, gea::Texture> Textures;
    std::unordered_map<short, gea::Physics> Physics;




    template<typename T>
    T* getComponent(short entityID);

    template<typename T>
    void addComponent(short entityID, const T &component);


};

/*** Template implementation ***/
///
/// @brief Retrives an entitys component
/// @param entityID
/// @return the component or nullptr if none exists for that entity
///
template<>
inline gea::Camera* registry::getComponent<gea::Camera>(short entityID)
{
    auto iterator = Cameras.find(entityID);

    if(iterator != Cameras.end())
        return &iterator->second; // pointer to the component found

    return nullptr;
}
template<>
inline gea::Transform* registry::getComponent<gea::Transform>(short entityID)
{
    auto iterator = Transforms.find(entityID);

    if(iterator != Transforms.end())
        return &iterator->second; // pointer to the component found

    return nullptr;
}
template<>
inline gea::Mesh* registry::getComponent<gea::Mesh>(short entityID)
{
    auto iterator = Meshes.find(entityID);

    if(iterator != Meshes.end())
        return &iterator->second; // pointer to the component found

    return nullptr;
}
template<>
inline gea::Texture* registry::getComponent<gea::Texture>(short entityID)
{
    auto iterator = Textures.find(entityID);

    if(iterator != Textures.end())
        return &iterator->second; // pointer to the component found

    return nullptr;
}
template<>
inline gea::Physics* registry::getComponent<gea::Physics>(short entityID)
{
    auto iterator = Physics.find(entityID);

    if (iterator != Physics.end())
        return &iterator->second;

    return nullptr;
}


/// @brief Adds a component to an entity
/// @param entityID
/// @param component to add
template<>
inline void registry::addComponent<gea::Camera>(short entityID, const gea::Camera &component)
{
    Cameras[entityID] = component;
}

template<>
inline void registry::addComponent<gea::Transform>(short entityID, const gea::Transform &component)
{
    Transforms[entityID] = component;
}

template<>
inline void registry::addComponent<gea::Mesh>(short entityID, const gea::Mesh &component)
{
    Meshes[entityID] = component;
}
template <>
inline void registry::addComponent<gea::Texture>(short entityID, const gea::Texture &component)
{
    Textures[entityID] = component;
}
template<>
inline void registry::addComponent<gea::Physics>(short entityID, const gea::Physics &component)
{
    Physics[entityID] = component;
}



#endif // REGISTRY_H
