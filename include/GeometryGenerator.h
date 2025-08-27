#ifndef GEOMETRY_GENERATOR_H
#define GEOMETRY_GENERATOR_H

#include <vector>

namespace GeometryGenerator {
    void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int sectors, int stacks);
}

#endif // GEOMETRY_GENERATOR_H
