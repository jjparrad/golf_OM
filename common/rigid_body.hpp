#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <iostream>

#include "transform.hpp"

class RigidBody {
public:
    Transform * transform;
    float mass = 1.0;
    glm::vec3 currentVelocity;
    
    RigidBody() = default;
    RigidBody(Transform *newTransform) : transform(newTransform) {};

    void applyGravity(float time) {
        glm::vec3 gravity = glm::vec3(0.0, -9.82, 0.0);
        currentVelocity += gravity * time;
        applySpeed(time, currentVelocity);
    };

    void applySpeed(float time, glm::vec3 velocity) {
        currentVelocity = velocity;
        transform->translate(currentVelocity * time);
    };
};

#endif // RIGID_BODY_H