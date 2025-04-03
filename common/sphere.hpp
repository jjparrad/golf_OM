#ifndef SHPERE_H
#define SHPERE_H

#include "mesh.hpp"
#include <vector>
#include <cstdlib>

// Include GLM
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Sphere : Mesh {
public:
    void createSphere(int nX = 20, int nY = 20) {
        for (int y = 0; y <= nY; y++) {
            for (int x = 0; x <= nX; x++) {
                float angleH = (2 * M_PI * x) / nX;
                float angleV = ((M_PI * (y)) / (nY)) - (M_PI / 2);

                float xValue = cos(angleH) * cos(angleV);
                float yValue = sin(angleH) * cos(angleV);
                float zValue = sin(angleV);

                vertices.push_back(glm::vec3(xValue, yValue, zValue));
            }
        }
    
        for (int y = 0; y <= nY; y++) {
            for (int x = 1; x <= nX; x++) {
                int currentVertexIndex = nX * y + x;
                int nextVertexIndex = nX * y + x + 1;
                int vertexBelowIndex = nX * (y + 1) + x;
                int vertexBelowNextIndex = nX * (y + 1) + x + 1;

                indices.push_back(currentVertexIndex);
                indices.push_back(nextVertexIndex);
                indices.push_back(vertexBelowNextIndex);

                indices.push_back(currentVertexIndex);
                indices.push_back(vertexBelowNextIndex);
                indices.push_back(vertexBelowIndex);
            }
        }

    }
};

#endif // SHPERE_H