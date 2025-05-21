#include "common/scene.hpp"

#include <vector>

#include "common/game_object.hpp"
#include "common/mesh.hpp"
#include "common/light.hpp"
#include "common/material.hpp"
#include "common/objloader.hpp"

Mesh loadModel(std::string filename) {
  std::vector<unsigned short> indices;
  std::vector<glm::vec3> indexed_vertices;
  std::vector<glm::vec2> textureCoords;
  std::vector<std::vector<unsigned short>> triangles;

  loadOFF(filename, indexed_vertices, indices, triangles);

  Mesh modelMesh(indices, indexed_vertices, textureCoords);
  return modelMesh;
}


void setScene(std::vector<Light> &lights, std::vector<GameObject*> &gameObjects) {
  std::vector<unsigned short> indices;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> texCoords;
  if (loadOBJ("../assets/golf_course/golf_course.obj",
            "../assets/golf_course/golf_course.mtl",
            indices, vertices, texCoords)) {
      Mesh courseMesh(indices, vertices, texCoords);

      GameObject *course = new GameObject(courseMesh);

      course->translate(glm::vec3(0.f, 0.0f , 0.f));
      course->scale(glm::vec3(1.0f, 1.0f, 1.0f));

      course->mesh.loadBuffers();
      gameObjects.push_back(course);

  }
  indices.clear();
  vertices.clear();
  texCoords.clear();

  Light firstLight = Light(glm::vec3(2.5,2.5,2.0), glm::vec3(1.0,1.0,1.0));
  lights.push_back(firstLight);

  Light secondLight = Light(glm::vec3(0.5,4.5,0.6), glm::vec3(0.7,0.5,0.1));
  lights.push_back(secondLight);

  Light thirdLight = Light(glm::vec3(0.5,1.5,0.0), glm::vec3(0.7,0.5,0.1));
  lights.push_back(thirdLight);

  std::string sphereMeshFilename("../assets/models/sphere.off");
  Material mat1 = Material( glm::vec3(1.0f,1.0f,1.0f), 0.0, 1.0, 0.0);
  Material mat2 = Material( glm::vec3(1.0f,1.0f,0.0f), 1.0, 0.0, 0.0);
  Material mat3 = Material( glm::vec3(0.0f,1.0f,1.0f), 1.0, 1.0, 0.0);
  Mesh sphereMesh1 = loadModel(sphereMeshFilename);
  Mesh sphereMesh2 = loadModel(sphereMeshFilename);
  Mesh sphereMesh3 = loadModel(sphereMeshFilename);
  sphereMesh1.material = mat1;
  sphereMesh2.material = mat2;
  sphereMesh3.material = mat3;

  GameObject* sphere = new GameObject(sphereMesh1);
  sphere->isPlayer = true;
  sphere->translate(glm::vec3(0.0f, 1.2f, 0.0f));
  sphere->lastPlayerspos = glm::vec3(0.0f, 1.0f, 0.0f);
  sphere->setTexCoordForSphere();
  sphere->scale(glm::vec3(0.04f, 0.04f, 0.04f));
  sphere->mesh.loadBuffers();
  gameObjects.push_back(sphere);

  /*GameObject* sphere2 = new GameObject(sphereMesh2);
  sphere2->isPlayer = true;
  sphere2->translate(glm::vec3(0.2f, 1.2f, 0.0f));
  sphere2->lastPlayerspos = glm::vec3(0.0f, 1.0f, 0.0f);
  sphere2->setTexCoordForSphere();
  sphere2->scale(glm::vec3(0.05f, 0.05f, 0.05f));
  sphere2->mesh.loadBuffers();
  gameObjects.push_back(sphere2);*/

  for (int i = 1 ; i < gameObjects.size(); i++){
      gameObjects[0]->children.push_back(gameObjects[i]);
      gameObjects[i]->parent = gameObjects[0];
  }

}



