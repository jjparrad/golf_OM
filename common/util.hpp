#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <vector>
#include <string>

// Déclarations externes pour éviter les conflits de linkage
extern std::vector<std::string> facesSky;
extern float skyboxVertices[108];

// Déclaration de la fonction
unsigned int loadCubemap(const std::vector<std::string>& faces, bool mipmap);

// Fonction utilitaire inline
inline void showFPS(GLFWwindow* window) {
    static const float timeBetweenUpdates = 0.5;
    static double last_time = glfwGetTime();
    static unsigned int nbframes = 0;
    static const std::string windowTitle("FPS - ");
    double current_time = glfwGetTime();

    nbframes++;
    if (current_time - last_time > timeBetweenUpdates) {
        glfwSetWindowTitle(window, (windowTitle + std::to_string((uint32_t)(nbframes / timeBetweenUpdates))).c_str());
        last_time = current_time;
        nbframes = 0;
    }
}

unsigned int loadTexture(const char *path);

void printVec3(glm::vec3 vector);