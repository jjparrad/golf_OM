#ifndef MESH_H
#define MESH_H

#include <GL/gl.h>
#include <glm/geometric.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "common/material.hpp"
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Mesh {
public:
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;

    glm::vec3 maxVertex;
    glm::vec3 minVertex;

    Material material;

    GLuint vertexbuffer;
    GLuint texturebuffer;
    GLuint elementbuffer;
    GLuint normalsbuffer;

    // Constructor / Clone
    Mesh() = default;
    Mesh(std::vector<unsigned short> new_indices, std::vector<glm::vec3> new_vertices, std::vector<glm::vec2> new_textureCoords){
        indices = new_indices;
        vertices = new_vertices;
        textureCoords = new_textureCoords;

        maxVertex = glm::vec3(vertices[0]);
        minVertex = glm::vec3(vertices[0]);
        for(glm::vec3 v : vertices) {
            if (v[0] > maxVertex[0]) maxVertex[0] = v[0];
            if (v[1] > maxVertex[1]) maxVertex[1] = v[1];
            if (v[2] > maxVertex[2]) maxVertex[2] = v[2];
            if (v[0] < minVertex[0]) minVertex[0] = v[0];
            if (v[1] < minVertex[1]) minVertex[1] = v[1];
            if (v[2] < minVertex[2]) minVertex[2] = v[2];
        }
        loadBuffers();
    };

    int hasTexture() {
        return textureCoords.size() > 0;        
    };

    void loadBuffers() {
        // Load it into a VBO
        GLuint newVertexbuffer;
        glGenBuffers(1, &newVertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, newVertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        
        // Generate a buffer for the indices as well
        GLuint newElementbuffer;
        glGenBuffers(1, &newElementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newElementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

        // buffer des normales
        recomputeNormals();
        GLuint newNormalbuffer;
        glGenBuffers(1, &newNormalbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newNormalbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &normals[0] , GL_STATIC_DRAW);

        normalsbuffer = newNormalbuffer;
        vertexbuffer = newVertexbuffer;
        elementbuffer = newElementbuffer;

        if (textureCoords.size() > 0) {
            // Texture Coords VBO
            GLuint newTexturebuffer;
            glGenBuffers(1, &newTexturebuffer);
            glBindBuffer(GL_ARRAY_BUFFER, newTexturebuffer);
            glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(glm::vec2), &textureCoords[0], GL_STATIC_DRAW);

            texturebuffer = newTexturebuffer;
        }
    };
    
    int getIndicesSize() {
        return indices.size();
    };

    void check() {
        std::cout << "Mesh alive" << std::endl;
    };

    bool isPositionInsideTriangle(glm::vec3 position, int triangleIdx) {
        int v1Idx = indices[triangleIdx + 0];
        int v2Idx = indices[triangleIdx + 1];
        int v3Idx = indices[triangleIdx + 2];

        glm::vec2 P = glm::vec2(position[0], position[2]);
        glm::vec2 A = glm::vec2(vertices[v1Idx][0], vertices[v1Idx][2]);
        glm::vec2 B = glm::vec2(vertices[v2Idx][0], vertices[v2Idx][2]);
        glm::vec2 C = glm::vec2(vertices[v3Idx][0], vertices[v3Idx][2]);

        glm::vec2 AB = B - A;
        glm::vec2 BC = C - B;
        glm::vec2 CA = A - C;

        glm::vec2 PA = A - P;
        glm::vec2 PB = B - P;
        glm::vec2 PC = C - P;

        float cross1 = AB[0] * PA[1] - AB[1] * PA[0];
        float cross2 = BC[0] * PB[1] - BC[1] * PB[0];
        float cross3 = CA[0] * PC[1] - CA[1] * PC[0];

        if (cross1 == 0 || cross2 == 0 || cross3 == 0) {
            return true;
        }

        if (cross1 > 0 && cross2 > 0 && cross3 > 0) {
            return true;
        }

        if (cross1 < 0 && cross2 < 0 && cross3 < 0) {
            return true;
        }

        return false;
    };

    float getHeightInPosition(glm::vec3 position) {
        for (int i = 0; i < indices.size(); i+=3) {
            if (isPositionInsideTriangle(position, i)) {
                int v1Idx = indices[i+0];
                int v2Idx = indices[i+1];
                int v3Idx = indices[i+2];

                glm::vec3 v1 = vertices[v1Idx];
                glm::vec3 v2 = vertices[v2Idx];
                glm::vec3 v3 = vertices[v3Idx];

                float xv1 = v1[0];
                float yv1 = v1[2];
                float xv2 = v2[0];
                float yv2 = v2[2];
                float xv3 = v3[0];
                float yv3 = v3[2];
                
                float px = position[0];
                float py = position[2];

                float w1 = ((yv2 - yv3) * (px - xv3) + (xv3 - xv2) * (py - yv3)) / ((yv2 - yv3) * (xv1 - xv3) + (xv3 - xv2) * (yv1 - yv3));
                float w2 = ((yv3 - yv1) * (px - xv3) + (xv1 - xv3) * (py - yv3)) / ((yv2 - yv3) * (xv1 - xv3) + (xv3 - xv2) * (yv1 - yv3));
                float w3 = 1 - w1 - w2;

                return vertices[v1Idx][1] * w1 + vertices[v2Idx][1] * w2 + vertices[v3Idx][1] * w3;
            }
        }

        std::cout << "No Projection" << std::endl;
        return position[1];
    };

    void setTexCoordForSphere() {
        textureCoords.clear();

        for (const glm::vec3& vertex : vertices) {
            // Conversion des coordonnées cartésiennes en coordonnées sphériques
            float theta = atan2(vertex.z, vertex.x);  // Angle autour de l'axe Y (longitude)
            float phi = acos(vertex.y);              // Angle depuis le pôle nord (latitude)

            // Normalisation des angles pour les UV
            float u = (theta + glm::pi<float>()) / (2.0f * glm::pi<float>()); // [0, 1]
            float v = phi / glm::pi<float>();                                 // [0, 1]

            textureCoords.push_back(glm::vec2(u, v));
        }
    }

    void recomputeNormals() {

        normals.resize(vertices.size(), glm::vec3(0.0f));

        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int i0 = indices[i];
            unsigned int i1 = indices[i + 1];
            unsigned int i2 = indices[i + 2];

            glm::vec3 v0 = vertices[i0];
            glm::vec3 v1 = vertices[i1];
            glm::vec3 v2 = vertices[i2];

            glm::vec3 e1 = v1 - v0;
            glm::vec3 e2 = v2 - v0;
            glm::vec3 faceNormal = glm::normalize(glm::cross(e1, e2));

            normals[i0] += faceNormal;
            normals[i1] += faceNormal;
            normals[i2] += faceNormal;
        }

        for (size_t i = 0; i < normals.size(); ++i) {
            normals[i] = glm::normalize(normals[i]);
        }
    }




};




#endif // MESH_H