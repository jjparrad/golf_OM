#pragma once


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>
#include "mesh.hpp" // Contient Mesh et Material
#include "game_object.hpp" // Contient la classe GameObject
#include "light.hpp"
#include "common/camera.hpp"

void drawObject3(Mesh mesh);

void initPRB(GLuint program);

void renderPRB(GLuint program,
               const std::vector<Light>& lights,
               Camera& camera,
               std::vector<GameObject*>& gameObjects,
               float deltaTime);