#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <iostream>

#include "transform.hpp"

class RigidBody {
public:
    Transform * transform;
    float mass = 1.0;
    
    RigidBody() = default;
    RigidBody(Transform *newTransform) : transform(newTransform) {};

    void applySpeed(float time, glm::vec3 velocity) {
        transform->translate(velocity * time);
    };
};

#endif // RIGID_BODY_H