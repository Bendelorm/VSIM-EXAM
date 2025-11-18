#ifndef TXTTOOBJ_H
#define TXTTOOBJ_H

#include <string>
#include <vector>

struct Point3D
{
    float x, y, z;
    float nx, ny, nz; // normals
    float u, v;       // UV texture coordinates
};

class TxtToObj
{
public:
    TxtToObj() = default;
    ~TxtToObj() = default;

    static void convertTxtToObj(const std::string& inputTxt, const std::string& outputObj);
};

#endif
