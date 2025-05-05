#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <iostream>

#include "transform.hpp"

class RigidBody {
public:
    float STOP_SPEED_LIMIT = 0.2;

    bool inGround = false;

    Transform * transform;
    float mass = 1.0;
    glm::vec3 currentVelocity;
    
    RigidBody() = default;
    RigidBody(Transform *newTransform) : transform(newTransform) {
        currentVelocity = glm::vec3(0.0);
    };

    void physicsLoop(float time) {
        transform->translate(currentVelocity * time);
    };

    void applyGravity(float time) {
        glm::vec3 gravity = glm::vec3(0.0, -9.82, 0.0);
        currentVelocity += gravity * time;
        applySpeed(time, currentVelocity);
    };

    void slowDown(float time) {
        if (!inGround) {
            return;
        }

        if (currentVelocity[0] < STOP_SPEED_LIMIT) {
            currentVelocity[0] = 0;
        } else {
            currentVelocity[0] *= 0.9;
        }

        if (currentVelocity[2] < STOP_SPEED_LIMIT) {
            currentVelocity[2] = 0;
        } else {
            currentVelocity[2] *= 0.9;
        }
    }

    void stopGravity() {
        currentVelocity[1] = 0.0;
        inGround = true;
    }

    void resetVelocity() {
        currentVelocity = glm::vec3(0.0);
    }

    void hit(float time, glm::vec3 direction, float force) {
        if (inGround) {
            inGround = false;
            currentVelocity = direction * force * time;
            applySpeed(time, currentVelocity);
        }
    };

    void applySpeed(float time, glm::vec3 velocity) {
        currentVelocity = velocity;
    };
};

#endif // RIGID_BODY_H