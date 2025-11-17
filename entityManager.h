#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "Entities/Entity.h"
#include <vector>

class entityManager{
private:
    std::vector<gea::Entity> entities;
public:
    gea::Entity createEntity(){
        gea::Entity e;
        entities.push_back(e);

        return e;
    }
};

#endif // ENTITYMANAGER_H
