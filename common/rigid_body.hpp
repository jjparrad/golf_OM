#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP

#include <glm/glm.hpp>
#include "transform.hpp"
#include "mesh.hpp"

class RigidBody {
public:
    float STOP_SPEED_LIMIT = 0.2f;
    bool inGround = false;

    Transform* transform = nullptr;
    float mass = 1.0f;
    glm::vec3 currentVelocity = glm::vec3(0.0f, -1.0f, 0.0f);

    RigidBody();
    RigidBody(Transform* newTransform);

    void physicsLoop(float time);
    void applyGravity(float time);
    void slowDown(float time);
    void stopGravity();
    void resetVelocity();
    void hit(float time, glm::vec3 direction, float force);
    void applySpeed(float time, glm::vec3 velocity);
};

bool areSpheresColliding(const Transform& a, const Transform& b, float radiusA, float radiusB);

void resolveSphereCollision(RigidBody& a, RigidBody& b, float radiusA, float radiusB);

bool rayIntersectsTriangle(const glm::vec3& origin, const glm::vec3& dir,
                           const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                           float& t);

bool rayIntersectsMesh(const glm::vec3& origin, const glm::vec3& dir,
                       const Mesh& m, float& tMin, glm::vec3& hitNormal);

#endif