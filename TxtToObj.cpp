#include "TxtToObj.h"
#include <QDebug>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

TxtToObj::TxtToObj()
    : xMin(0), xMax(0), yMin(0), yMax(0), zMin(0), zMax(0),
    cellSize(5.0), numCellsX(0), numCellsY(0){
}

bool TxtToObj::loadPointsFromTxt(const std::string& filename, int skipEveryN) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        qDebug() << "Error: Failed to open file" << QString::fromStdString(filename);
        return false;
    }

    rawPoints.clear();
    std::string line;
    int lineCount = 0;
    int pointsLoaded = 0;

    qDebug() << "Loading point cloud from" << QString::fromStdString(filename);
    if (skipEveryN > 1) {
        qDebug() << "Reading every" << skipEveryN << "point to reduce data";
    }

    while (std::getline(file, line)) {
        lineCount++;

        if (line.empty() || line[0] == '#') continue;

        if (lineCount % skipEveryN != 0) continue;

        std::istringstream iss(line);
        double x, y, z;

        if (iss >> x >> y >> z) {
            rawPoints.push_back(Vertex3D(x, y, z));
            pointsLoaded++;

            if (pointsLoaded % 10000 == 0) {
                qDebug() << "Loaded" << pointsLoaded << "points...";
            }
        }
    }

    file.close();
    qDebug() << "Successfully loaded" << rawPoints.size() << "points";

    return !rawPoints.empty();
}
void TxtToObj::centerToOrigin()
{
    if (vertices.empty()) return;

    // Calculate center point of the terrain
    double centerX = (xMin + xMax) / 2.0;
    double centerY = (yMin + yMax) / 2.0;
    double centerZ = (zMin + zMax) / 2.0;

    qDebug() << "Centering mesh to origin...";
    qDebug() << "Original center:" << centerX << "," << centerY << "," << centerZ;

    // Translate all vertices so the center is at origin
    for (auto& v : vertices) {
        v.x -= centerX;
        v.y -= centerY;
        v.z -= centerZ;
    }

    // Update bounding box
    xMin -= centerX;
    xMax -= centerX;
    yMin -= centerY;
    yMax -= centerY;
    zMin -= centerZ;
    zMax -= centerZ;

    qDebug() << "New bounding box:";
    qDebug() << "  X: [" << xMin << "," << xMax << "]";
    qDebug() << "  Y: [" << yMin << "," << yMax << "]";
    qDebug() << "  Z: [" << zMin << "," << zMax << "]";
}

void TxtToObj::calculateBoundingBox() {
    if (rawPoints.empty()) {
        qDebug() << "Warning: No points to calculate bounding box from";
        return;
    }

    xMin = yMin = zMin = std::numeric_limits<double>::max();
    xMax = yMax = zMax = std::numeric_limits<double>::lowest();

    for (const auto& point : rawPoints) {
        if (point.x < xMin) xMin = point.x;
        if (point.x > xMax) xMax = point.x;
        if (point.y < yMin) yMin = point.y;
        if (point.y > yMax) yMax = point.y;
        if (point.z < zMin) zMin = point.z;
        if (point.z > zMax) zMax = point.z;
    }

    qDebug() << "Bounding box:";
    qDebug() << "  X: [" << xMin << "," << xMax << "] range =" << (xMax - xMin) << "m";
    qDebug() << "  Y: [" << yMin << "," << yMax << "] range =" << (yMax - yMin) << "m";
    qDebug() << "  Z: [" << zMin << "," << zMax << "] range =" << (zMax - zMin) << "m";
}

void TxtToObj::createRegularGrid(double cellSize_) {
    cellSize = cellSize_;

    qDebug() << "\n=== Creating Regular Grid ===";
    qDebug() << "Cell size:" << cellSize << "x" << cellSize << "meters";

    calculateBoundingBox();

    numCellsX = static_cast<int>(std::ceil((xMax - xMin) / cellSize));
    numCellsY = static_cast<int>(std::ceil((yMax - yMin) / cellSize));

    qDebug() << "Grid dimensions:" << numCellsX << "x" << numCellsY
             << "cells (" << (numCellsX * numCellsY) << "total)";

    grid.clear();
    grid.resize(numCellsX);
    for (int i = 0; i < numCellsX; ++i) {
        grid[i].resize(numCellsY);
    }

    distributePointsToGrid();
    fillMissingCells();
    createGridVertices();
}

void TxtToObj::distributePointsToGrid() {
    qDebug() << "Distributing points to grid cells...";

    int pointsProcessed = 0;
    int pointsOutOfBounds = 0;

    for (const auto& point : rawPoints) {
        int i = static_cast<int>((point.x - xMin) / cellSize);
        int j = static_cast<int>((point.y - yMin) / cellSize);

        if (i >= 0 && i < numCellsX && j >= 0 && j < numCellsY) {
            GridCell& cell = grid[i][j];
            cell.sumZ += point.z;
            cell.count++;
            cell.hasData = true;
            pointsProcessed++;
        } else {
            pointsOutOfBounds++;
        }
    }

    int cellsWithData = 0;
    for (int i = 0; i < numCellsX; ++i) {
        for (int j = 0; j < numCellsY; ++j) {
            if (grid[i][j].hasData) cellsWithData++;
        }
    }

    qDebug() << "Processed" << pointsProcessed << "points into" << cellsWithData << "grid cells";
    if (pointsOutOfBounds > 0) {
        qDebug() << "Warning:" << pointsOutOfBounds << "points outside grid bounds";
    }
}

void TxtToObj::createGridVertices()
{
    qDebug() << "Creating vertices from grid cells...";

    vertices.clear();
    vertices.reserve(numCellsX * numCellsY);

    for (int i = 0; i < numCellsX; ++i) {
        for (int j = 0; j < numCellsY; ++j) {
            const GridCell& cell = grid[i][j];

            double x = xMin + (i + 0.5) * cellSize;
            double y = yMin + (j + 0.5) * cellSize;
            double z = cell.hasData ? cell.getAverageHeight() : 0.0;

            vertices.push_back(Vertex3D(x, y, z));
        }
    }

    qDebug() << "Created" << vertices.size() << "vertices";
    centerToOrigin();
}
double TxtToObj::interpolateHeight(int i, int j) {
    // Use weighted average based on distance
    // Closer neighbors have more influence
    double sumZ = 0.0;
    double sumWeights = 0.0;

    // Search in a larger radius for smoother interpolation
    int searchRadius = 3;

    for (int di = -searchRadius; di <= searchRadius; ++di) {
        for (int dj = -searchRadius; dj <= searchRadius; ++dj) {
            int ni = i + di;
            int nj = j + dj;

            // Check bounds
            if (ni >= 0 && ni < numCellsX && nj >= 0 && nj < numCellsY) {
                if (grid[ni][nj].hasData) {
                    // Calculate distance (Manhattan distance is faster)
                    double distance = std::abs(di) + std::abs(dj);
                    if (distance == 0) distance = 0.1; // Avoid division by zero

                    // Weight: closer cells have more influence
                    double weight = 1.0 / (distance * distance);

                    sumZ += grid[ni][nj].getAverageHeight() * weight;
                    sumWeights += weight;
                }
            }
        }
    }

    // Return weighted average
    if (sumWeights > 0.0) {
        return sumZ / sumWeights;
    }

    return 0.0;
}

void TxtToObj::fillMissingCells() {
    qDebug() << "Filling missing cells with interpolated values...";

    int cellsFilled = 0;

    // More passes for smoother interpolation
    for (int pass = 0; pass < 10; ++pass) {
        std::vector<std::pair<int, int>> cellsToFill;

        // Find cells without data
        for (int i = 0; i < numCellsX; ++i) {
            for (int j = 0; j < numCellsY; ++j) {
                if (!grid[i][j].hasData) {
                    // Check if any neighbor has data (within radius 3)
                    bool hasNeighborWithData = false;
                    for (int di = -3; di <= 3 && !hasNeighborWithData; ++di) {
                        for (int dj = -3; dj <= 3 && !hasNeighborWithData; ++dj) {
                            int ni = i + di;
                            int nj = j + dj;
                            if (ni >= 0 && ni < numCellsX && nj >= 0 && nj < numCellsY) {
                                if (grid[ni][nj].hasData) {
                                    hasNeighborWithData = true;
                                }
                            }
                        }
                    }

                    if (hasNeighborWithData) {
                        cellsToFill.push_back({i, j});
                    }
                }
            }
        }

        // Fill these cells
        for (const auto& cell : cellsToFill) {
            int i = cell.first;
            int j = cell.second;
            double height = interpolateHeight(i, j);

            grid[i][j].sumZ = height;
            grid[i][j].count = 1;
            grid[i][j].hasData = true;
            cellsFilled++;
        }

        if (cellsToFill.empty()) break;

        qDebug() << "Pass" << (pass + 1) << "- filled" << cellsToFill.size() << "cells";
    }

    qDebug() << "Total filled:" << cellsFilled << "cells with interpolated data";
}

int TxtToObj::getVertexIndex(int i, int j) const {
    return i * numCellsY + j;
}

void TxtToObj::generateTriangulation() {
    qDebug() << "\n=== Generating Regular Triangulation ===";

    triangles.clear();
    triangles.reserve((numCellsX - 1) * (numCellsY - 1) * 2);

    // Split each grid square into two triangles
    for (int i = 0; i < numCellsX - 1; ++i) {
        for (int j = 0; j < numCellsY - 1; ++j) {
            int v1 = getVertexIndex(i, j);
            int v2 = getVertexIndex(i + 1, j);
            int v3 = getVertexIndex(i, j + 1);
            int v4 = getVertexIndex(i + 1, j + 1);

            // Create triangles for all squares (no more holes!)
            triangles.push_back(Triangle(v1, v2, v3));
            triangles.push_back(Triangle(v2, v4, v3));
        }
    }

    qDebug() << "Created" << triangles.size() << "triangles";
}

Normal TxtToObj::calculateTriangleNormal(const Triangle& tri) const {
    const Vertex3D& v0 = vertices[tri.v1];
    const Vertex3D& v1 = vertices[tri.v2];
    const Vertex3D& v2 = vertices[tri.v3];

    // Edge vectors
    float ax = static_cast<float>(v1.x - v0.x);
    float ay = static_cast<float>(v1.y - v0.y);
    float az = static_cast<float>(v1.z - v0.z);

    float bx = static_cast<float>(v2.x - v0.x);
    float by = static_cast<float>(v2.y - v0.y);
    float bz = static_cast<float>(v2.z - v0.z);

    // Cross product
    Normal n;
    n.x = ay * bz - az * by;
    n.y = az * bx - ax * bz;
    n.z = ax * by - ay * bx;

    n.normalize();
    return n;
}

void TxtToObj::calculateNormals() {
    qDebug() << "\n=== Calculating Vertex Normals ===";

    normals.clear();
    normals.resize(vertices.size(), Normal(0, 0, 0));

    // Accumulate normals from all adjacent triangles
    for (const auto& tri : triangles) {
        Normal triNormal = calculateTriangleNormal(tri);

        normals[tri.v1].x += triNormal.x;
        normals[tri.v1].y += triNormal.y;
        normals[tri.v1].z += triNormal.z;

        normals[tri.v2].x += triNormal.x;
        normals[tri.v2].y += triNormal.y;
        normals[tri.v2].z += triNormal.z;

        normals[tri.v3].x += triNormal.x;
        normals[tri.v3].y += triNormal.y;
        normals[tri.v3].z += triNormal.z;
    }

    // Normalize all accumulated normals
    for (auto& n : normals) {
        n.normalize();
    }

    qDebug() << "Calculated smooth normals for" << normals.size() << "vertices";
}

void TxtToObj::generateUVCoordinates() {
    qDebug() << "\n=== Generating UV Texture Coordinates ===";

    uvs.clear();
    uvs.reserve(vertices.size());

    double xRange = xMax - xMin;
    double yRange = yMax - yMin;

    if (xRange < 0.0001) xRange = 1.0;
    if (yRange < 0.0001) yRange = 1.0;

    for (const auto& v : vertices) {
        float u = static_cast<float>((v.x - xMin) / xRange);
        float v_coord = static_cast<float>((v.y - yMin) / yRange);

        u = std::max(0.0f, std::min(1.0f, u));
        v_coord = std::max(0.0f, std::min(1.0f, v_coord));

        uvs.push_back(UV(u, v_coord));
    }

    qDebug() << "Generated UV coordinates for" << uvs.size() << "vertices";
}

bool TxtToObj::exportToOBJ(const std::string& filename) {
    qDebug() << "\n=== Exporting to OBJ file ===";
    qDebug() << "Writing to:" << QString::fromStdString(filename);

    std::ofstream file(filename);
    if (!file.is_open()) {
        qDebug() << "Error: Could not create output file";
        return false;
    }

    // Write header
    file << "# Terrain mesh generated from point cloud\n";
    file << "# Vertices: " << vertices.size() << "\n";
    file << "# Triangles: " << triangles.size() << "\n";
    file << "# Cell size: " << cellSize << " meters\n";
    file << "#\n\n";

    // Write vertices
    file << "# Vertex positions\n";
    for (const auto& v : vertices) {
        file << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }
    file << "\n";

    // Write texture coordinates
    file << "# Texture coordinates\n";
    for (const auto& uv : uvs) {
        file << "vt " << uv.u << " " << uv.v << "\n";
    }
    file << "\n";

    // Write normals
    file << "# Vertex normals\n";
    for (const auto& n : normals) {
        file << "vn " << n.x << " " << n.y << " " << n.z << "\n";
    }
    file << "\n";

    // Write faces (OBJ uses 1-based indexing)
    file << "# Faces\n";
    for (const auto& tri : triangles) {
        file << "f "
             << (tri.v1 + 1) << "/" << (tri.v1 + 1) << "/" << (tri.v1 + 1) << " "
             << (tri.v2 + 1) << "/" << (tri.v2 + 1) << "/" << (tri.v2 + 1) << " "
             << (tri.v3 + 1) << "/" << (tri.v3 + 1) << "/" << (tri.v3 + 1) << "\n";
    }

    file.close();

    qDebug() << "Successfully exported mesh";
    qDebug() << "Vertices:" << vertices.size();
    qDebug() << "Normals:" << normals.size();
    qDebug() << "UVs:" << uvs.size();
    qDebug() << "Triangles:" << triangles.size();

    return true;
}
