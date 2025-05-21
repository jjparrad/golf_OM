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

float closestDistanceSegmentTriangle(const glm::vec3& segA, const glm::vec3& segB,
                                     const glm::vec3& triA, const glm::vec3& triB, const glm::vec3& triC,
                                     glm::vec3& outNormal, float& outT);


bool sphereCast(const glm::vec3& start, const glm::vec3& end, float radius, const Mesh& mesh, float& t, glm::vec3& normal);

glm::vec3 closestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

void processPhysique2(GameObject* current,GameObject* terrain,float deltaTime );


void findGroundPosition(const glm::vec3& position, GameObject* terrain, float radius, 
                       glm::vec3& outPosition, float& outDistance);