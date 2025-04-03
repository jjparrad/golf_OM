#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <common/game_object.hpp>


void processZoom(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up,int &focusedObject, std::vector<GameObject*> gameObjects);

void processWASD(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up,int &focusedObject, std::vector<GameObject*> gameObjects);

void processSHIFTSPACE(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up,int &focusedObject, std::vector<GameObject*> gameObjects);

void processARROWS(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up,int &focusedObject, std::vector<GameObject*> gameObjects);

void processInput(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up,int &focusedObject, std::vector<GameObject*> gameObjects);


