#include "mesh.hpp"
#include "game_object.hpp"
#include "rigid_body.hpp"

void processPhysique(GameObject* current,GameObject* terrain, float deltaTime);

void spheresCollision( std::vector<GameObject*> gameObjects, float sphereRadius);