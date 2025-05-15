#pragma once

#include <GL/glew.h>     
#include <GLFW/glfw3.h>  
#include <glm/glm.hpp>    
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>

#include "common/material.hpp"
#include "common/mesh.hpp"

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
    GLuint vaoID;


    Mesh() = default;
    Mesh(std::vector<unsigned short> new_indices, 
         std::vector<glm::vec3> new_vertices, 
         std::vector<glm::vec2> new_textureCoords);


    int hasTexture();
    void loadBuffers();
    int getIndicesSize();
    void check();
    bool isPositionInsideTriangle(glm::vec3 position, int triangleIdx);
    float getHeightInPosition(glm::vec3 position);
    void setTexCoordForSphere();
    void recomputeNormals();
};

