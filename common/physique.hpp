#include "mesh.hpp"
#include "game_object.hpp"
#include "rigid_body.hpp"

void processPhysique(GameObject* current,GameObject* terrain, float deltaTime);

void spheresCollision( std::vector<GameObject*> gameObjects, float sphereRadius);

bool areSpheresColliding(const Transform& a, const Transform& b, float radiusA, float radiusB);

void resolveSphereCollision(RigidBody& a, RigidBody& b, float radiusA, float radiusB);

bool rayIntersectsTriangle(const glm::vec3& origin, const glm::vec3& dir,
                           const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                           float& t);

bool rayIntersectsMesh(const glm::vec3& origin, const glm::vec3& dir,
                       const Mesh& m, float& tMin, glm::vec3& hitNormal);