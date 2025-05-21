#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "game_object.hpp"
#include "mesh.hpp"
#include "camera.hpp"


void drawObject2(Mesh mesh);

void initTerrainShader(GLuint programID, GLuint textureHeightmap, GLuint texture1);

void renderTerrain(GLuint programID, GameObject* terrain, Camera& camera, bool isFocused);

void initSkybox(GLuint& vao, GLuint& vbo, GLuint& cubemapTexture, const std::vector<std::string>& faces);

void renderSkybox(GLuint vao, GLuint cubemapTexture, GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);