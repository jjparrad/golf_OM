// Include standard headers
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/game_object.hpp>
#include "common/input.hpp"
#include "common/surface.hpp"


Mesh loadModel(std::string filename);
void setScene();
void loadGameObject(GameObject object, GLuint vertexbuffer, GLuint texturebuffer, GLuint elementbuffer);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* TEXTURE_LOW_PATH = "../assets/textures/grass.png";
const char* TEXTURE_MID_PATH = "../assets/textures/rock.png";
const char* TEXTURE_HIGH_PATH = "../assets/textures/snowrocks.png";

// const char* HEIGHTMAP_PATH = "../assets/textures/heightmap_rocky.png";
// const char* HEIGHTMAP_PATH = "../assets/textures/heightmap_mountain.png";
// const char* HEIGHTMAP_PATH = "../assets/textures/heightmap_1024x1024.png";
const char* HEIGHTMAP_PATH = "../assets/textures/heightmap_1.jpg";

const char* TEXTURE_EARTH_PATH = "../assets/textures/s1.png";


unsigned char *heightmapData;
int heightmapWidth, heightmapHeight, heightmapNrChannels;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, 5.0f, 5.0f);
glm::vec3 camera_target = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

float DISTANCE_LOW_RESOLUTION = 10.0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float inputLastTime = 0.0f;

// Surface settings
float SURFACE_DISTANCE_DELTA = 0.1;

// Scene objects
std::vector<GameObject*> gameObjects;
int focusedObject = -1;
/*******************************************************************************/

void drawObject(Mesh mesh) {

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexbuffer);
        glVertexAttribPointer(0,  3,  GL_FLOAT, GL_FALSE, 0, (void*)0);

        if (mesh.hasTexture()) {
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.texturebuffer);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        }

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.elementbuffer);
        
        // Draw the triangles !
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    mesh.getIndicesSize(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );
        

        glDisableVertexAttribArray(0);
    }


/*******************************************************************************/

unsigned int loadTexture(const char* path) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // X
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Y
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        if (path == HEIGHTMAP_PATH) {
            heightmapData = data;
            heightmapWidth = width;
            heightmapHeight = height;
            heightmapNrChannels = nrChannels;
        } else {
            stbi_image_free(data);
        }

        return textureID;
    } else {
        std::cout << "Failed to load texture" << std::endl;
        return 0;
    }
}

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, "GLFW", NULL, NULL);
    if(window == NULL) {
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, SCR_WIDTH/2, SCR_HEIGHT/2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "../src/shaders/vertex_shader.glsl", "../src/shaders/fragment_shader.glsl" );

    GLuint programBallID = LoadShaders( "../src/shaders/ball_vertex_shader.glsl", "../src/shaders/ball_fragment_shader.glsl" );


    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> textureCoords;

    // Generate texture
    glEnable(GL_TEXTURE_2D);
    GLuint textureHeightmap = loadTexture(HEIGHTMAP_PATH);
    GLuint texture1 = loadTexture(TEXTURE_LOW_PATH);
    GLuint texture2 = loadTexture(TEXTURE_MID_PATH);
    GLuint texture3 = loadTexture(TEXTURE_HIGH_PATH);

    GLuint texture_earth = loadTexture(TEXTURE_EARTH_PATH);




    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    GLuint texturebuffer;
    glGenBuffers(1, &texturebuffer);
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);

    setScene();
    int terrain = 0;

    // For speed computation
    lastFrame = glfwGetTime();


    
    float currentTime = glfwGetTime();
    float inputDeltaTime = currentTime - inputLastTime;
    inputLastTime = currentTime;

    int nbFrames = 0;
    do {
        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        processInput(window, deltaTime, inputLastTime, camera_position, camera_target, camera_up, focusedObject, gameObjects);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





        //===========Gestion Camera ============================================================
        glm::mat4 model = glm::mat4(1.0f);

        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up
        glm::mat4 view = glm::lookAt(
            camera_position,
            camera_target + camera_position,
            camera_up
        );

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms
        glm::mat4 mvp = projection * view * model;

        //================================================================================


        //2nd shader

        glUseProgram(programBallID);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture_earth);
        glUniform1i(glGetUniformLocation(programBallID, "texture_earth"), 0);

        GLuint mvp_uniform = glGetUniformLocation(programBallID, "MVP");
        glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, &mvp[0][0]);

        for(int i = 1; i < gameObjects.size(); i++){
            Mesh sp = gameObjects[i]->mesh;
            drawObject(sp);
        }



        // Use our shader
        glUseProgram(programID);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, textureHeightmap);
        glUniform1i(glGetUniformLocation(programID, "heightmap"), 0);

        glActiveTexture(GL_TEXTURE0 + 1);  
        // glBindTexture(GL_TEXTURE_2D, textureHeightmap);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(programID, "textureImgLow"), 1);

        glActiveTexture(GL_TEXTURE0 + 2);  
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(programID, "textureImgMid"), 2);

        glActiveTexture(GL_TEXTURE0 + 3);  
        glBindTexture(GL_TEXTURE_2D, texture3);
        glUniform1i(glGetUniformLocation(programID, "textureImgHigh"), 3);

        for (int i = 0; i < gameObjects.size(); i++) {
            if (i == 0) {
                glUniform1i(glGetUniformLocation(programID, "useHeight"), 1);
            } else {
                glUniform1i(glGetUniformLocation(programID, "useHeight"), 0);
            }

            if (i == focusedObject) {
                glUniform1i(glGetUniformLocation(programID, "isFocused"), 1);
            } else {
                glUniform1i(glGetUniformLocation(programID, "isFocused"), 0);
            }

            Transform* transform = &gameObjects[i]->transform;

            if (i != terrain) {
                float terrainHeight = gameObjects[terrain]->adjustHeight(gameObjects[i]);
                if (terrainHeight < transform->position[1] - SURFACE_DISTANCE_DELTA) {
                    gameObjects[i]->applyGravity(deltaTime);
                } else if (terrainHeight > transform->position[1] + SURFACE_DISTANCE_DELTA) {
                    transform->setYPosition(terrainHeight);
                }
                // gameObjects[i]->rigidBody.applySpeed(deltaTime, glm::vec3(0.0, 0.0, 1.0));
            }
            
            glm::mat4 model = gameObjects[i]->getTransformation();

            // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up
            glm::mat4 view = glm::lookAt(
                camera_position,
                camera_target + camera_position,
                camera_up
            );

            // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

            // Send our transformation to the currently bound shader,
            // in the "Model View Projection" to the shader uniforms
            glm::mat4 mvp = projection * view * model;
            GLuint mvp_uniform = glGetUniformLocation(programID, "MVP");
            glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, &mvp[0][0]);

        Mesh terrain = gameObjects[0]->mesh;
        drawObject(terrain);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );


    for (int i = 0; i < gameObjects.size(); i++) {
        // Cleanup VBO and shader
        Mesh mesh = gameObjects[i]->mesh;
        glDeleteBuffers(1, &mesh.vertexbuffer);
        glDeleteBuffers(1, &mesh.texturebuffer);
        glDeleteBuffers(1, &mesh.elementbuffer);
    }

    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}


Mesh loadModel(std::string filename) {
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<std::vector<unsigned short>> triangles;
    
    loadOFF(filename, indexed_vertices, indices, triangles);
    
    Mesh modelMesh(indices, indexed_vertices, textureCoords);
    return modelMesh;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void setScene() {
    GameObject* surface = generateSurface(heightmapHeight, heightmapWidth, heightmapNrChannels,heightmapData);
    gameObjects.push_back(surface);
    surface->translate(glm::vec3(-5.0f, 0.0f, -5.0f));
    surface->applytransform();
    
    std::string sphereMeshFilename("../models/sphere.off");
    std::string sphereMeshLowFilename("../models/suzanne.off");

    Mesh sphereMesh = loadModel(sphereMeshLowFilename);
    GameObject* sphere = new GameObject(sphereMesh);
    sphere->translate(glm::vec3(5.0f, 1.0f, 0.0f));
    sphere->setTexCoordForSphere();
    sphere->scale(glm::vec3(1.5f, 1.5f, 1.5f));
    sphere->applytransform();
    gameObjects.push_back(sphere);

    GameObject* sphere2 = new GameObject(sphereLowMesh);
    sphere2->setTexCoordForSphere();
    sphere2->translate(glm::vec3(0.3f, 5.0f, 0.0f));
    sphere2->scale(glm::vec3(1.5f, 1.5f, 1.5f));
    sphere2->applytransform();
    gameObjects.push_back(sphere2);


}