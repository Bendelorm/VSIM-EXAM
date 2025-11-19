#ifndef TXTTOOBJ_H
#define TXTTOOBJ_H

#include <vector>
#include <string>
#include <cmath>

struct Vertex3D {
    double x, y, z;
    Vertex3D() : x(0), y(0), z(0) {}
    Vertex3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
};

struct Triangle {
    int v1, v2, v3;
    Triangle(int a, int b, int c) : v1(a), v2(b), v3(c) {}
};

struct Normal {
    float x, y, z;
    Normal() : x(0), y(0), z(1) {}
    Normal(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    void normalize() {
        float len = std::sqrt(x*x + y*y + z*z);
        if (len > 0.0001f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }
};

struct UV {
    float u, v;
    UV() : u(0), v(0) {}
    UV(float u_, float v_) : u(u_), v(v_) {}
};

struct GridCell {
    double sumZ;
    int count;
    bool hasData;

    GridCell() : sumZ(0.0), count(0), hasData(false) {}

    double getAverageHeight() const {
        return (count > 0) ? sumZ / count : 0.0;
    }
};

class TxtToObj {
public:
    TxtToObj();

    bool loadPointsFromTxt(const std::string& filename, int skipEveryN = 1);
    void createRegularGrid(double cellSize);
    void generateTriangulation();
    void calculateNormals();
    void generateUVCoordinates();
    bool exportToOBJ(const std::string& filename);

    size_t getRawPointCount() const { return rawPoints.size(); }
    size_t getVertexCount() const { return vertices.size(); }
    size_t getTriangleCount() const { return triangles.size(); }

private:
    std::vector<Vertex3D> rawPoints;

    double xMin, xMax, yMin, yMax, zMin, zMax;

    double cellSize;
    int numCellsX, numCellsY;
    std::vector<std::vector<GridCell>> grid;

    std::vector<Vertex3D> vertices;
    std::vector<Triangle> triangles;
    std::vector<Normal> normals;
    std::vector<UV> uvs;


    double interpolateHeight(int i, int j);
    void fillMissingCells();
    void centerToOrigin();
    void calculateBoundingBox();
    void distributePointsToGrid();
    void createGridVertices();
    int getVertexIndex(int i, int j) const;
    Normal calculateTriangleNormal(const Triangle& tri) const;
};

#endif // TXTTOOBJ_H
