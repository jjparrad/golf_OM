#include "camera.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <common/game_object.hpp>


void processZoom(GLFWwindow *window,float deltaTime, float inputLastTime, Camera &camera,int &focusedObject, std::vector<GameObject*> gameObjects);

void processWASD(GLFWwindow *window,float deltaTime, float inputLastTime, Camera &camera,int &focusedObject, std::vector<GameObject*> gameObjects);

void processSHIFTSPACE(GLFWwindow *window,float deltaTime, float inputLastTime, Camera &camera,int &focusedObject, std::vector<GameObject*> gameObjects);

void processARROWS(GLFWwindow *window,float deltaTime, float inputLastTime, Camera &camera,int &focusedObject, std::vector<GameObject*> gameObjects);

void processInput(GLFWwindow *window,float deltaTime, float inputLastTime, Camera &camera,int &focusedObject, std::vector<GameObject*> gameObjects);


