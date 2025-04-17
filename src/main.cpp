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
#include "common/material.hpp"
#include "common/light.hpp"
#include "common/util.hpp"


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
float SURFACE_DISTANCE_DELTA = 0.5;

// Scene objects
std::vector<GameObject*> gameObjects;
std::vector<Light> lights;
int focusedObject = -1;
/*******************************************************************************/

void drawObject(Mesh mesh) {
    // Position
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Texture coordinates
    if (mesh.hasTexture()) {
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.texturebuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // Normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.normalsbuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.elementbuffer);

    // Draw the triangles
    glDrawElements(
        GL_TRIANGLES,
        mesh.getIndicesSize(),
        GL_UNSIGNED_SHORT,
        (void*)0
    );

}
/*************************************************************************** */





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

unsigned int loadCubemap(std::vector<std::string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false); // Ne pas retourner les textures de la skybox
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}




/*******************************************************************************/ 

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
    glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

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
    GLuint programSky = LoadShaders( "../src/shaders/sky_vertex_shader.glsl", "../src/shaders/sky_fragment_shader.glsl" );

    GLuint programID = LoadShaders( "../src/shaders/vertex_shader.glsl", "../src/shaders/fragment_shader.glsl" );

    GLuint programBallID = LoadShaders( "../src/shaders/ball_vertex_shader.glsl", "../src/shaders/ball_fragment_shader.glsl" );

    GLuint programPRB = LoadShaders("../src/shaders/PRB_vertex_shader.glsl", "../src/shaders/PRB_fragment_shader.glsl");


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

    std::vector<std::string> faces{
        "../assets/cubemap_sky/right.png",   
        "../assets/cubemap_sky/left.png",
        
            
        "../assets/cubemap_sky/up.png",      
        "../assets/cubemap_sky/down.png",
        
        "../assets/cubemap_sky/back.png",   
        "../assets/cubemap_sky/front.png",   
        
          
    };

    GLuint textureSky = loadCubemap(faces);

    GLuint skyboxVAO, skyboxVBO;


    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);


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
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    processInput(window, deltaTime, inputLastTime, camera_position, camera_target, camera_up, focusedObject, gameObjects);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //=========== Gestion Camera ================
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(camera_position, camera_target + camera_position, camera_up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 mvp = projection * view * model;



    //========= 1er shader ===============
    glUseProgram(programID);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, textureHeightmap);
    glUniform1i(glGetUniformLocation(programID, "heightmap"), 0);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glUniform1i(glGetUniformLocation(programID, "textureImgLow"), 1);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glUniform1i(glGetUniformLocation(programID, "textureImgMid"), 2);

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glUniform1i(glGetUniformLocation(programID, "textureImgHigh"), 3);


    model = gameObjects[0]->getTransformation();
    view = glm::lookAt(camera_position, camera_target + camera_position, camera_up);
    projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    mvp = projection * view * model;

    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &mvp[0][0]);


    glUniform1i(glGetUniformLocation(programID, "isFocused"), 0 == focusedObject ? 1 : 0);
    Mesh terrainMesh = gameObjects[0]->mesh;
    drawObject(terrainMesh);

    //printf("focusedObject :%d \n",focusedObject);



    /*//========= 2nd shader ============
    glUseProgram(programBallID);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture_earth);
    glUniform1i(glGetUniformLocation(programBallID, "texture_earth"), 0);

    GLuint mvp_uniform = glGetUniformLocation(programBallID, "MVP");
    glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, &mvp[0][0]);


    for (int i = 1; i < gameObjects.size(); i++) {

        glUniform1i(glGetUniformLocation(programBallID, "useHeight"), i == 0 ? 1 : 0);
        glUniform1i(glGetUniformLocation(programBallID, "isFocused"), i == focusedObject ? 1 : 0);
    
        Transform* transform = &gameObjects[i]->transform;

        if (i != terrain) {
            float terrainHeight = gameObjects[terrain]->adjustHeight(gameObjects[i]);
            if (terrainHeight < transform->position[1] - SURFACE_DISTANCE_DELTA) {
                gameObjects[i]->applyGravity(deltaTime);
            } else if (terrainHeight > transform->position[1] + SURFACE_DISTANCE_DELTA) {
                transform->setYPosition(terrainHeight);
            }
        }

        glm::mat4 model = gameObjects[i]->getTransformation();
        glm::mat4 view = glm::lookAt(camera_position, camera_target + camera_position, camera_up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, &mvp[0][0]);

        Mesh sp = gameObjects[i]->mesh;
        drawObject(sp);
    }*/


    // PRB =====================================================================
    glUseProgram(programPRB);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture_earth);
    glUniform1i(glGetUniformLocation(programPRB, "texture_earth"), 0);

    GLuint mvp_uniform = glGetUniformLocation(programPRB, "MVP");
    glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, &mvp[0][0]);




    GLuint light_pos_uniform = glGetUniformLocation(programPRB, "lightPosition");
    GLuint light_col_uniform = glGetUniformLocation(programPRB, "lightColor");

    glm::vec3 l_pos = lights[0].pos;
    glm::vec3 l_col = lights[0].color;



    glUniform3f(light_pos_uniform, l_pos[0], l_pos[1], l_pos[2]);
    glUniform3f(light_col_uniform, l_col[0], l_col[1], l_col[2]);

    GLuint cam_pos_uniform = glGetUniformLocation(programPRB, "camPos");
    glUniform3f(cam_pos_uniform, camera_position[0], camera_position[1], camera_position[2]);

    //printf("%f, %f, %f \n", camera_position[0], camera_position[1], camera_position[2]);


    for (int i = 1 ; i < gameObjects.size(); i++){

        Transform* transform = &gameObjects[i]->transform;
        glm::mat4 model = gameObjects[i]->getTransformation();
        glm::mat4 view = glm::lookAt(camera_position, camera_target + camera_position, camera_up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, &mvp[0][0]);

        GLuint modelLoc = glGetUniformLocation(programPRB, "model");
        GLuint viewLoc = glGetUniformLocation(programPRB, "view");
        GLuint projLoc = glGetUniformLocation(programPRB, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

        Material currentMat = gameObjects[i]->mesh.material;


        GLuint metal_f_uniform = glGetUniformLocation(programPRB, "metallic");
        glUniform1f(metal_f_uniform, currentMat.metal_f);

        GLuint rough_f_uniform = glGetUniformLocation(programPRB, "roughness");
        glUniform1f(rough_f_uniform, currentMat.rough_f);

        GLuint ao_uniform = glGetUniformLocation(programPRB, "ao");
        glUniform1f(ao_uniform, currentMat.ao);
        
        GLuint albedo_uniform = glGetUniformLocation(programPRB, "albedo");
        glUniform3f(albedo_uniform, currentMat.albedo[0], currentMat.albedo[1], currentMat.albedo[2]);


        Mesh sp = gameObjects[i]->mesh;
        drawObject(sp);

    }


    //====== sky shader =============
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glUseProgram(programSky);

    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(glGetUniformLocation(programSky, "view"), 1, GL_FALSE, &viewNoTranslation[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programSky, "projection"), 1, GL_FALSE, &projection[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureSky);
    glUniform1i(glGetUniformLocation(programSky, "skybox"), 0);

    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);



    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

// ========= Cleanup ============
for (int i = 0; i < gameObjects.size(); i++) {
    Mesh mesh = gameObjects[i]->mesh;
    glDeleteBuffers(1, &mesh.vertexbuffer);
    glDeleteBuffers(1, &mesh.texturebuffer);
    glDeleteBuffers(1, &mesh.elementbuffer);
}

glDeleteProgram(programID);
glDeleteVertexArrays(1, &VertexArrayID);
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
    surface->translate(glm::vec3(-2.0f, 0.0f, -1.0f));
    surface->applytransform();
    
    std::string sphereMeshFilename("../models/sphere.off");
    std::string sphereMeshLowFilename("../models/suzanne.off");



    Light firstLight = Light(glm::vec3(0,2.5,1.0), glm::vec3(1.0,1.0,1.0));
    lights.push_back(firstLight);

    Material firstMat = Material( glm::vec3(1.0f,0.0,0.0), 0.2, 0.8, 1.0);
    Mesh sphereMesh = loadModel(sphereMeshLowFilename);
    sphereMesh.material = firstMat;

    GameObject* sphere = new GameObject(sphereMesh);
    sphere->translate(glm::vec3(5.0f, 1.0f, 0.0f));
    sphere->setTexCoordForSphere();
    sphere->scale(glm::vec3(0.5f, 0.5f, 0.5f));
    sphere->mesh.loadBuffers();
    //sphere->applytransform();
    gameObjects.push_back(sphere);

    GameObject* sphere2 = new GameObject(sphereMesh);
    sphere2->setTexCoordForSphere();
    sphere2->translate(glm::vec3(0.3f, 2.0f, 0.0f));
    sphere2->scale(glm::vec3(1.0f, 1.0f, 1.0f));
    sphere2->mesh.loadBuffers();
    //sphere2->applytransform();
    gameObjects.push_back(sphere2);

    //setupSkybox();
}