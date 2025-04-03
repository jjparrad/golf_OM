#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <common/mesh.hpp>
#include <common/game_object.hpp>

int NVertexPerSide = 50;

GameObject* generateSurface(int heightmapHeight, int heightmapWidth, int heightmapNrChannels,unsigned char *heightmapData) {
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> textureCoords;

    int N = NVertexPerSide;
    float size = 10; // size of the terrain

    float vertexSize = size / (float) (N - 1);

    for(int i = 0; i < N; i++) {
        float xCoord = vertexSize * i;
        float u = (float) (i / (float)(N - 1));
        for(int j = 0; j < N; j++) {
            float v = (float) (j / (float)(N - 1));
            glm::vec2 textureCoord(u, v);
            textureCoords.push_back(textureCoord);

            float zCoord = vertexSize * (float) j;

            int textureIndex = (int)(v * (heightmapHeight - 1)) * heightmapWidth + (int)(u * (heightmapWidth - 1));

            float yCoord = ((float) heightmapData[textureIndex*heightmapNrChannels]) / 255.0;
            glm::vec3 position(xCoord, yCoord, zCoord);
            indexed_vertices.push_back(position);
        }
    }

    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - 1; j++) {
            short topLeft = i * N + j;
            short topRight = i * N + j + 1;
            short bottomLeft = (i + 1) * N + j;
            short bottomRight = (i + 1) * N + j + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    Mesh surfaceMesh(indices, indexed_vertices, textureCoords);
    GameObject* surface = new GameObject(surfaceMesh);

    return surface;
}


