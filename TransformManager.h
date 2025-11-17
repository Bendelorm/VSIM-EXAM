#ifndef TRANSFORMMANAGER_H
#define TRANSFORMMANAGER_H

#include <glm/glm.hpp>

namespace gea
{

// Forward declaration
struct Transform;

class TransformManager
{


public:
    // === Position Functions ===
    static glm::vec3 getPosition(short entityID);
    static void setPosition(short entityID, const glm::vec3& position);
    static void translate(short entityID, const glm::vec3& offset);

    // === Rotation Functions (in Degrees) ===

    static glm::vec3 getRotation(short entityID);
    static void setRotation(short entityID, const glm::vec3& rotation);
    static void rotate(short entityID, const glm::vec3& deltaRotation);

    // === Scale Functions ===

    static glm::vec3 getScale(short entityID);
    static void setScale(short entityID, const glm::vec3& scale);
    static void setUniformScale(short entityID, float scale);

    // === Matrix Building ===

    static glm::mat4 getModelMatrix(short entityID);
    static glm::mat4 buildModelMatrix(const Transform& transform);
};

} // namespace gea

#endif // TRANSFORMSYSTEM_H
