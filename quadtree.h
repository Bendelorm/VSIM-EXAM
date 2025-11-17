#ifndef QUADTREE_H
#define QUADTREE_H

#include <glm/glm.hpp>

namespace gea
{
class QuadTree
{
private:
    glm::vec3 mBottomLeftPoint;
    glm::vec3 mBottomRightPoint;
    glm::vec3 mTopRightPoint;
    glm::vec3 mTopLeftPoint;

    gea::QuadTree* mBottomLeftQuad;
    gea::QuadTree* mBottomRightQuad;
    gea::QuadTree* mTopRightQuad;
    gea::QuadTree* mTopLeftQuad;

    bool isLeaf() const;

public:
    QuadTree();
    QuadTree(const glm::vec3 &bottomLeftPoint, const glm::vec3 &bottomRightPoint, const glm::vec3 &topRightPoint, const glm::vec3 &topLeftPoint);
    void Subdivide(int n);

    //The Rest has not been implemented, as it uses different logic for getting the entity's position

    //QuadTree* Insert(const gea::Entity &entity);
    //QuadTree* Find(const glm::vec3 &point);
};
} // end of namespace gea

#endif // QUADTREE_H
