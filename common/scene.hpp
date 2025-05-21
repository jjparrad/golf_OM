// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include "game_object.hpp"
#include "light.hpp"

Mesh loadModel(std::string filename);

void setScene(std::vector<Light> &lights, std::vector<GameObject*> &gameObjects);