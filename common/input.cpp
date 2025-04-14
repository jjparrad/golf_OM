#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <common/game_object.hpp>

#include "common/input.hpp"



void processZoom(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up){
    //Camera zoom in and out
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera_position += cameraSpeed * camera_target;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera_position -= cameraSpeed * camera_target;
}

void processWASD(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up,int &focusedObject, std::vector<GameObject*> gameObjects){
    float cameraSpeed = 2.5 * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 a = glm::vec3(camera_target.x, 0, camera_target.z);
        if (focusedObject == -1) {
            camera_position += cameraSpeed * a;
        } else {
            gameObjects[focusedObject]->translate(a * glm::vec3(0.1));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 a = glm::vec3(camera_target.x, 0, camera_target.z);
        if (focusedObject == -1) {
            camera_position -= cameraSpeed * a;
        } else {
            gameObjects[focusedObject]->translate(a * glm::vec3(-0.1));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 left = glm::cross(camera_up, camera_target);
        if (focusedObject == -1) {
            camera_position -= cameraSpeed * left;
        } else {
            gameObjects[focusedObject]->translate(left * glm::vec3(-0.1));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 left = glm::cross(camera_up, camera_target);
        if (focusedObject == -1) {
            camera_position += cameraSpeed * left;
        } else {
            gameObjects[focusedObject]->translate(left * glm::vec3(0.1));
        }
    }

}

void processSHIFTSPACE(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up,int &focusedObject, std::vector<GameObject*> gameObjects){
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (focusedObject == -1) { 
            camera_position += (0.5f * cameraSpeed) * camera_up;
        } else {
            gameObjects[focusedObject]->translate(glm::vec3(0, 0.1, 0));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (focusedObject == -1) {
            camera_position -= (0.5f * cameraSpeed) * camera_up;
        } else {
            gameObjects[focusedObject]->translate(glm::vec3(0, -0.1, 0));
        }
    }
}

void processARROWS(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up,int &focusedObject, std::vector<GameObject*> gameObjects){
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (focusedObject == -1) {
            camera_target += (0.5f * cameraSpeed) * camera_up;
        } else {
            gameObjects[focusedObject]->applytranslate(glm::vec3(0, 0, -0.1));
            
        }
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (focusedObject == -1) {
            camera_target -= (0.5f * cameraSpeed) * camera_up;
        } else {
            gameObjects[focusedObject]->applytranslate(glm::vec3(0, 0, 0.1));
            
        }
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (focusedObject == -1) {
            glm::vec3 left = glm::cross(camera_up, camera_target);
            camera_target -= (0.5f * cameraSpeed) * left;
        } else {
            gameObjects[focusedObject]->applytranslate(glm::vec3(0.1, 0, 0));
            
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (focusedObject == -1) {
            glm::vec3 left = glm::cross(camera_up, camera_target);
            camera_target += (0.5f * cameraSpeed) * left;
        } else {
            gameObjects[focusedObject]->applytranslate(glm::vec3(-0.1, 0, 0));
        
        }
    }
}


void processInput(GLFWwindow *window,float deltaTime, float inputLastTime, glm::vec3 &camera_position,glm::vec3 &camera_target,glm::vec3 &camera_up,int &focusedObject, std::vector<GameObject*> gameObjects) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    //Camera zoom in and out
    processZoom(window, deltaTime, inputLastTime, camera_position, camera_target, camera_up);
    processWASD(window, deltaTime, inputLastTime, camera_position, camera_target, camera_up,focusedObject,gameObjects);
    processSHIFTSPACE(window, deltaTime, inputLastTime, camera_position, camera_target, camera_up,focusedObject,gameObjects);
    processARROWS(window, deltaTime, inputLastTime, camera_position, camera_target, camera_up,focusedObject,gameObjects);


    static bool tabPressed = false;

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        float currentTime = glfwGetTime();
        float inputDeltaTime = currentTime - inputLastTime;
        if (inputDeltaTime < 0.2) return;

        inputLastTime = currentTime;
        tabPressed = true;

        focusedObject++;
        if (focusedObject >= gameObjects.size()) {
            focusedObject = -1;
        }
    } 
    else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        tabPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        std::cout <<"the camera is :" <<camera_position.x << ", " << camera_position.y << ", " << camera_position.z << std::endl;
        
    }
}


