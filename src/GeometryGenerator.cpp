#include "GeometryGenerator.h"
#include <cmath>

namespace GeometryGenerator {
    void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int sectors, int stacks)
    {
        vertices.clear();
        indices.clear();

        const float PI = 3.14159265359f;
        float x, y, z, xy;
        float nx, ny, nz, lengthInv = 1.0f / radius;

        float sectorStep = 2 * PI / sectors;
        float stackStep = PI / stacks;
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stacks; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectors; ++j)
            {
                sectorAngle = j * sectorStep;

                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);
            }
        }

        for (int i = 0; i < stacks; ++i)
        {
            for (int j = 0; j < sectors; ++j)
            {
                int k1 = i * (sectors + 1) + j;
                int k2 = k1 + sectors + 1;

                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}
