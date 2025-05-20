#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <common/game_object.hpp>

#include "common/input.hpp"
#include <stdio.h>

void processZoom(GLFWwindow *window,float deltaTime, float currentFrame, Camera &camera){
    //Camera zoom in and out
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.position += cameraSpeed * camera.target;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.position -= cameraSpeed * camera.target;
}

void processWASD(GLFWwindow *window,float deltaTime, float currentFrame, Camera &camera, int &focusedObject, std::vector<GameObject*> gameObjects){
    float cameraSpeed = 2.5 * deltaTime;

    if (focusedObject == -1) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            glm::vec3 a = glm::vec3(camera.target.x, 0, camera.target.z);
            camera.position += cameraSpeed * a;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            glm::vec3 a = glm::vec3(camera.target.x, 0, camera.target.z);
            camera.position -= cameraSpeed * a;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            glm::vec3 left = glm::cross(camera.up, camera.target);
            camera.position -= cameraSpeed * left;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            glm::vec3 left = glm::cross(camera.up, camera.target);
            camera.position += cameraSpeed * left;
        }
    } else {
        glm::vec3 a = glm::vec3(camera.target.x, 0, camera.target.z);
        glm::vec3 left = glm::cross(camera.up, camera.target);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            gameObjects[focusedObject]->translate(a * glm::vec3(0.1));
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            gameObjects[focusedObject]->translate(a * glm::vec3(-0.1));
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            gameObjects[focusedObject]->translate(left * glm::vec3(-0.1));
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            gameObjects[focusedObject]->translate(left * glm::vec3(0.1));
        }
    }


}

void processSHIFTSPACE(GLFWwindow *window,float deltaTime, float currentFrame, Camera &camera, int &focusedObject, std::vector<GameObject*> gameObjects){
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (focusedObject == -1) { 
            camera.position += (0.5f * cameraSpeed) * camera.up;
        } else {
            gameObjects[focusedObject]->translate(glm::vec3(0, 0.1, 0));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (focusedObject == -1) {
            camera.position -= (0.5f * cameraSpeed) * camera.up;
        } else {
            gameObjects[focusedObject]->translate(glm::vec3(0, -0.1, 0));
        }
    }
}

void processARROWS(GLFWwindow *window,float deltaTime, float currentFrame, Camera &camera ,int &focusedObject, std::vector<GameObject*> gameObjects){
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        glm::vec3 a = glm::vec3(camera.target.x, 0, camera.target.z);
        if (focusedObject == -1) {
            camera.target += (0.5f * cameraSpeed) * camera.up;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        glm::vec3 a = glm::vec3(camera.target.x, 0, camera.target.z);
        if (focusedObject == -1) {
            camera.target -= (0.5f * cameraSpeed) * camera.up;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        glm::vec3 left = glm::cross(camera.up, camera.target);
        if (focusedObject == -1) {
            glm::vec3 left = glm::cross(camera.up, camera.target);
            camera.target -= (0.5f * cameraSpeed) * left;
        } else {
            camera.orbitalRotation(-1);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        glm::vec3 left = glm::cross(camera.up, camera.target);
        if (focusedObject == -1) {
            glm::vec3 left = glm::cross(camera.up, camera.target);
            camera.target += (0.5f * cameraSpeed) * left;
        } else {
            camera.orbitalRotation(1);
        }
    }
}

void processGameInputs(GLFWwindow *window, float deltaTime, float currentFrame, Camera &camera ,int &focusedObject, std::vector<GameObject*> gameObjects){
    static bool hitPressed = false;
    static float pressTime = 0;
    
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !hitPressed) {
        hitPressed = true;
        pressTime = currentFrame;
    } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && hitPressed) {
            hitPressed = false;
            float force = currentFrame - pressTime;
            force = force > 1.0 ? 1.0 : force;
            float interpolatedForce = 1 - std::pow((1 - force), 3);
            glm::vec3 a = glm::vec3(camera.target.x, 0.0, camera.target.z);
            gameObjects[focusedObject]->rigidBody.hit(deltaTime, glm::normalize(a), 800.0 * force);
            gameObjects[focusedObject]->rigidBody.ismoving = true;
            printf("force: %f \n", force);
    }
}


void processInput(GLFWwindow *window, float deltaTime, float currentFrame, Camera &camera, int &focusedObject, std::vector<GameObject*> gameObjects) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    //Camera zoom in and out
    processZoom(window, deltaTime, currentFrame, camera);
    processWASD(window, deltaTime, currentFrame, camera,focusedObject,gameObjects);
    processSHIFTSPACE(window, deltaTime, currentFrame, camera,focusedObject,gameObjects);
    processARROWS(window, deltaTime, currentFrame, camera,focusedObject,gameObjects);

    processGameInputs(window, deltaTime, currentFrame, camera,focusedObject,gameObjects);
    
    static bool tabPressed = false;

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        float currentTime = glfwGetTime();
        tabPressed = true;

        focusedObject++;
        if (focusedObject >= gameObjects.size()) {
            focusedObject = -1;
        } else if (focusedObject != -1 && gameObjects[focusedObject]->isPlayer) {
            camera.setTarget(gameObjects[focusedObject]);
        }
    } 
    else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        tabPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        std::cout <<"the camera is :" << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << std::endl;
    }
}


