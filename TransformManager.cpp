#include "TransformManager.h"
#include "engineinit.h"
#include "Components/Components.h"
#include <glm/gtc/matrix_transform.hpp>

namespace gea
{


// === Position Functions ===

glm::vec3 TransformManager::getPosition(short entityID)
{
    auto* transform = EngineInit::registry.getComponent<Transform>(entityID);
    if (transform){
        return transform->mPosition;
    }
    return glm::vec3(0.0f);
}

void TransformManager::setPosition(short entityID, const glm::vec3& position)
{
    auto* transform = EngineInit::registry.getComponent<Transform>(entityID);
    if (transform) {
        transform->mPosition = position;
    }
}

void TransformManager::translate(short entityID, const glm::vec3& offset)
{
    auto* transform = EngineInit::registry.getComponent<Transform>(entityID);
    if (transform) {
        transform->mPosition += offset;
    }
}

// === Rotation Functions ===

glm::vec3 TransformManager::getRotation(short entityID)
{
    auto* transform = EngineInit::registry.getComponent<Transform>(entityID);
    if (transform) {
        return transform->mRotation;
    }
    return glm::vec3(0.0f);
}

void TransformManager::setRotation(short entityID, const glm::vec3& rotation)
{
    auto* transform = EngineInit::registry.getComponent<Transform>(entityID);
    if (transform) {
        transform->mRotation = rotation;
    }
}

void TransformManager::rotate(short entityID, const glm::vec3& deltaRotation)
{
    auto* transform = EngineInit::registry.getComponent<Transform>(entityID);
    if (transform) {
        transform->mRotation += deltaRotation;
    }
}

// === Scale Functions ===

glm::vec3 TransformManager::getScale(short entityID)
{
    auto* transform = EngineInit::registry.getComponent<Transform>(entityID);
    if (transform) {
        return transform->mScale;
    }
    return glm::vec3(1.0f);
}

void TransformManager::setScale(short entityID, const glm::vec3& scale)
{
    auto* transform = EngineInit::registry.getComponent<Transform>(entityID);
    if (transform) {
        transform->mScale = scale;
    }
}

void TransformManager::setUniformScale(short entityID, float scale)
{
    setScale(entityID, glm::vec3(scale));
}

// === Matrix Building ===

glm::mat4 TransformManager::getModelMatrix(short entityID)
{
    auto* transform = EngineInit::registry.getComponent<Transform>(entityID);
    if (!transform) {
        return glm::mat4(1.0f);  // Identity matrix
    }

    return buildModelMatrix(*transform);
}

glm::mat4 TransformManager::buildModelMatrix(const Transform& transform)
{
    // Start with identity
    glm::mat4 model = glm::mat4(1.0f);

    // Apply translation
    model = glm::translate(model, transform.mPosition);

    // Apply rotation (in degrees, order: X -> Y -> Z)
    model = glm::rotate(model, glm::radians(transform.mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Apply scale
    model = glm::scale(model, transform.mScale);

    return model;
}

} // namespace gea
