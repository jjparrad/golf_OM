#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP

#include <glm/glm.hpp>
#include "transform.hpp"
#include "mesh.hpp"

class RigidBody {
public:
    float STOP_SPEED_LIMIT = 0.2f;
    bool inGround = false;
    bool ismoving = true;

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
    void applySlopeForce(float time, const glm::vec3& groundNormal);
};





#endif