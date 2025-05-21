// Include standard headers
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow *window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

#include "common/textRender.hpp"
#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/game_object.hpp>
#include "common/camera.hpp"
#include "common/player.hpp"
#include "common/input.hpp"
#include "common/surface.hpp"
#include "common/material.hpp"
#include "common/light.hpp"
#include "common/util.hpp"
#include "common/precalcul.hpp"
#include "common/rigid_body.hpp"
#include "common/physique.hpp"
#include "common/terrainRender.hpp"
#include "common/PBRrender.hpp"
#include "common/scene.hpp"



void changeActivePlayer(int &currentPlayer);
void loadGameObject(GameObject object, GLuint vertexbuffer,
                    GLuint texturebuffer, GLuint elementbuffer);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

const char *TEXTURE_LOW_PATH = "../assets/golf_course/Textures/colormap.png";
const char *TEXTURE_MID_PATH = "../assets/textures/rock.png";
const char *TEXTURE_HIGH_PATH = "../assets/textures/snowrocks.png";

// const char* HEIGHTMAP_PATH = "../assets/textures/heightmap_rocky.png";
// const char* HEIGHTMAP_PATH = "../assets/textures/heightmap_mountain.png";
// const char* HEIGHTMAP_PATH = "../assets/textures/heightmap_1024x1024.png";
const char *HEIGHTMAP_PATH = "../assets/textures/heightmap_1.jpg";

const char *TEXTURE_EARTH_PATH = "../assets/textures/chess.jpg";
// const char *TEXTURE_EARTH_PATH = "../assets/textures/s1.png";


float DISTANCE_LOW_RESOLUTION = 10.0;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
float inputLastTime = 0.0f;

// Surface settings
float SURFACE_DISTANCE_DELTA = 0.08;

// Scene objects
std::vector<GameObject*> gameObjects;
std::vector<glm::vec3> lastPlayerspos;
std::vector<Light> lights;
int focusedObject = -1;

//text


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
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                 GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLFW", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr,
            "Failed to open GLFW window. If you have an Intel GPU, they are "
            "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
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
  //glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders( "../src/shaders/vertex_shader.glsl", "../src/shaders/fragment_shader.glsl" );

  GLuint programPRB = LoadShaders("../src/shaders/PRB_vertex_shader.glsl", "../src/shaders/PRB_fragment_shader.glsl");

  GLuint programSky = LoadShaders( "../src/shaders/sky_vertex_shader.glsl", "../src/shaders/sky_fragment_shader.glsl" );

  GLuint programText = LoadShaders("../src/shaders/text_vertex_shader.glsl", "../src/shaders/text_fragment_shader.glsl");


  InitTextRendering("../assets/fonts/arial.ttf");

  glm::mat4 textprojection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    glUseProgram(programText);
    glUniformMatrix4fv(glGetUniformLocation(programText, "projection"), 1, GL_FALSE, glm::value_ptr(textprojection));

  std::vector<unsigned short> indices;
  std::vector<glm::vec3> indexed_vertices;
  std::vector<glm::vec2> textureCoords;


  // Generate texture
  glEnable(GL_TEXTURE_2D);
  GLuint textureHeightmap = loadTexture(HEIGHTMAP_PATH);
  GLuint texture1 = loadTexture(TEXTURE_LOW_PATH);


  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  GLuint texturebuffer;
  glGenBuffers(1, &texturebuffer);
  GLuint elementbuffer;
  glGenBuffers(1, &elementbuffer);

  setScene(lights, gameObjects);

  //init terrain

  initTerrainShader(programID,textureHeightmap, texture1);


  // init sky VAO
  GLuint cubemapTexture;
  GLuint skyboxVAO, skyboxVBO;
  initSkybox(skyboxVAO, skyboxVBO, cubemapTexture, facesSky);


  //init PBR
  initPRB(programPRB);

  int terrain = 0;
  Camera camera;
  Player player;
  
  // For speed computation
  lastFrame = glfwGetTime();

   

  GLint prevVAO;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);

  
  // For speed computation
  lastFrame = glfwGetTime();
  float currentTime = glfwGetTime();
  float inputDeltaTime = currentTime - inputLastTime;
  inputLastTime = currentTime;

  int nbFrames = 0;


  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


// ========================================================
  glm::vec3 camera_target;
  glm::vec3 camera_position;
  glm::vec3 camera_up;

  int lastFocusedObject = focusedObject;
  do {
    // Measure speed
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    showFPS(window);

    if (player.finishTurn()) {
      changeActivePlayer(focusedObject);
      player.setPlayer(gameObjects[focusedObject]);
      camera.setTarget(gameObjects[focusedObject]);
    } else {
      processInput(window, deltaTime, currentFrame, camera, focusedObject, gameObjects);
    }

    if (lastFocusedObject != focusedObject) {
      lastFocusedObject = focusedObject;
      if (focusedObject != -1) {
        player.setPlayer(gameObjects[focusedObject]);
      }
    }

    camera.updatePosition();

    camera_target = camera.target;
    camera_position = camera.position;
    camera_up = camera.up;
    player.saveCameraDirection(camera_target);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);




    //=========== Gestion Camera ================
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(camera.position, camera.target + camera.position, camera_up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 mvp = projection * view * model;


    // ========== Rendu skybox ==========

    renderSkybox(skyboxVAO, cubemapTexture, programSky, view, projection);


    //========= 1er shader ===============


    renderTerrain(programID, gameObjects[0], camera, 0 == focusedObject ? 1 : 0);


    // PRB =====================================================================

    renderPRB(programPRB, lights, camera, gameObjects, deltaTime); // la physique se trouve dans cette fonction (et physique.cpp)

    

    RenderText(programText, playerText(focusedObject), 25.0f, 25.0f, 0.6f, glm::vec3(0.0f, 0.0f, 0.0f));

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

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



// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

void changeActivePlayer(int &currentPlayer) {
  int focusedObject = currentPlayer + 1;
  if (focusedObject >= gameObjects.size()) {
      focusedObject = 0;
  }
  while (!gameObjects[focusedObject]->isPlayer) {
    focusedObject++;
    if (focusedObject >= gameObjects.size()) {
        focusedObject = 0;
    }
  }

  currentPlayer = focusedObject;
}

// setScene se trouve dans scene.cpp

