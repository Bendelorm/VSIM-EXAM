#include "quadtree.h"

namespace gea
{
QuadTree::QuadTree() {}

QuadTree::QuadTree(const glm::vec3 &bottomLeftPoint, const glm::vec3 &bottomRightPoint, const glm::vec3 &topRightPoint, const glm::vec3 &topLeftPoint)
    : mBottomLeftPoint{bottomLeftPoint}, mBottomRightPoint{bottomRightPoint}, mTopRightPoint{topRightPoint}, mTopLeftPoint{topLeftPoint}
{
}

void QuadTree::Subdivide(int n)
{
    if (n > 0)
    {
        glm::vec3 v1 = (mBottomLeftPoint + mBottomRightPoint) * 0.5f;
        glm::vec3 v2 = (mBottomRightPoint + mTopRightPoint) * 0.5f;
        glm::vec3 v3 = (mTopRightPoint + mTopLeftPoint) * 0.5f;
        glm::vec3 v4 = (mTopLeftPoint + mBottomLeftPoint) * 0.5f;
        glm::vec3 m = (mBottomLeftPoint + mTopRightPoint) * 0.5f;

        n--;

        mBottomLeftQuad = new QuadTree(mBottomLeftPoint, v1, m, v4);
        mBottomRightQuad = new QuadTree(v1, mBottomRightPoint, v2, m);
        mTopRightQuad = new QuadTree(m, v2, mTopRightPoint, v3);
        mTopLeftQuad = new QuadTree(v4, m, v3, mTopLeftPoint);
        mBottomLeftQuad->Subdivide(n);
        mBottomRightQuad->Subdivide(n);
        mTopRightQuad->Subdivide(n);
        mTopLeftQuad->Subdivide(n);
    }
}

/*QuadTree* QuadTree::Insert(const gea::Entity &entity)
{
    if (isLeaf())
    {
        mEntities.push_back(entity);
        return this;
    }

    glm::vec3 m = (mBottomLeftPoint + mTopRightPoint) * 0.5f;
    if (entity.)
}*/

} //end of namespace gea
