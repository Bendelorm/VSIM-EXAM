#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include "../Components/Components.h"

//Need namespace, since we start to get naming collisions with other code
namespace gea
{

//Struct used to store a Component Type and its index
struct EntityComponents {
    ComponentTypes mComponentType;
    short mComponentIndex{-1};
};

class Entity {
public:
    Entity() : mEntityID(++nextID) {}   //pre-increment - start at 1

    ~Entity();

    std::size_t mEntityID;              //unique ID for each Entity
    //std::vector<EntityComponents> mComponents;  //holds the components for this Entity

    inline static std::size_t nextID; // should be increasing by 1 for each entity made
};

 //initializes the static class value

} // namespace gea

#endif // ENTITY_H
