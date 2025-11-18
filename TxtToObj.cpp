#include "TxtToObj.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <iostream>

// In your Renderer implementation
#include "TxtToObj.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <iostream>

// In your Renderer implementation
void TxtToObj::convertTxtToObj(const std::string& inputTxt, const std::string& outputObj)
{
    std::vector<glm::vec3> points;

    //Read all points
    std::ifstream inFile(inputTxt);
    if (!inFile.is_open())
    {
        throw std::runtime_error("Could not open input file: " + inputTxt);
    }

    std::string line;
    while (std::getline(inFile, line))
    {
        std::istringstream iss(line);
        glm::vec3 point;
        if (iss >> point.x >> point.y >> point.z)
        {
            points.push_back(point);
        }
    }
    inFile.close();

    if (points.empty())
    {
        throw std::runtime_error("No points found in input file!");
    }

    // Find bounding box
    glm::vec3 minBounds(std::numeric_limits<float>::max());
    glm::vec3 maxBounds(std::numeric_limits<float>::lowest());

    for (const auto& p : points) {
        minBounds = glm::min(minBounds, p);
        maxBounds = glm::max(maxBounds, p);
    }

    // Calculate center
    glm::vec3 center = (minBounds + maxBounds) / 2.0f;
    glm::vec3 size = maxBounds - minBounds;

    std::cout << "Original center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
    std::cout << "Bounding box size: (" << size.x << ", " << size.y << ", " << size.z << ")" << std::endl;

    std::ofstream outFile(outputObj);
    if (!outFile.is_open())
    {
        throw std::runtime_error("Could not open output file: " + outputObj);
    }

    // first line: Point count
    outFile << "# Total points: " << points.size() << "\n";
    outFile << "# Point cloud from LAZ files\n";
    outFile << "# Centered at origin\n";
    outFile << "# Original center: " << center.x << " " << center.y << " " << center.z << "\n\n";

    // Write all vertices
    for (const auto& p : points)
    {
        glm::vec3 centered = p - center;
        outFile << "v " << centered.x << " " << centered.y << " " << centered.z << "\n";
    }

    // Write a single dummy UV coordinate
    outFile << "\nvt 0.0 0.0\n";

    // Write degenerate triangles (same vertex 3 times) so tinyobj will load indices
    outFile << "\n";
    for (size_t i = 1; i <= points.size(); i++) {
        outFile << "f " << i << "/1 " << i << "/1 " << i << "/1\n";
    }

    outFile.close();

    std::cout << "Converted '" << inputTxt << "' to '" << outputObj << "' (centered at origin)" << std::endl;
    std::cout << "Total points: " << points.size() << std::endl;
}
