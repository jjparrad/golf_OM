#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <iostream>
#include "rigid_body.hpp"
#include "transform.hpp"

RigidBody::RigidBody() = default;

RigidBody::RigidBody(Transform* newTransform) : transform(newTransform) {
    currentVelocity = glm::vec3(0.0f);
}

void RigidBody::physicsLoop(float time) {
    transform->translate(currentVelocity * time);
}

void RigidBody::applyGravity(float time) {
    glm::vec3 gravity = glm::vec3(0.0f, -9.82f, 0.0f);
    currentVelocity += gravity * time;
    applySpeed(time, currentVelocity);
}

void RigidBody::slowDown(float time) {
    if (!inGround) return;

    if (currentVelocity[0] < STOP_SPEED_LIMIT) {
        currentVelocity[0] = 0;
    } else {
        currentVelocity[0] *= 0.9f;
    }

    if (currentVelocity[2] < STOP_SPEED_LIMIT) {
        currentVelocity[2] = 0;
    } else {
        currentVelocity[2] *= 0.9f;
    }
}

void RigidBody::stopGravity() {
    currentVelocity[1] = 0.0f;
    inGround = true;
}

void RigidBody::resetVelocity() {
    currentVelocity = glm::vec3(0.0f);
}

void RigidBody::hit(float time, glm::vec3 direction, float force) {
    if (inGround) {
        inGround = false;
        currentVelocity = direction * force * time;
        applySpeed(time, currentVelocity);
    }
}

void RigidBody::applySpeed(float time, glm::vec3 velocity) {
    currentVelocity = velocity;
}


bool areSpheresColliding(const Transform& a, const Transform& b, float radiusA, float radiusB) {
    float distance = glm::distance(a.position, b.position);
    return distance < (radiusA + radiusB);
}

void resolveSphereCollision(RigidBody& a, RigidBody& b, float radiusA, float radiusB) {
    glm::vec3 posA = a.transform->position;
    glm::vec3 posB = b.transform->position;
    glm::vec3 delta = posB - posA;
    float dist = glm::length(delta);
    float overlap = (radiusA + radiusB) - dist;

    if (overlap > 0.0f) {
        glm::vec3 collisionNormal = glm::normalize(delta);
        
        glm::vec3 correction = collisionNormal * (overlap / 2.0f);
        a.transform->position -= correction;
        b.transform->position += correction;

 
        glm::vec3 relativeVelocity = b.currentVelocity - a.currentVelocity;
        float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

        if (velocityAlongNormal > 0) return; 

        float restitution = 1.0f;
        float impulseScalar = -(1.0f + restitution) * velocityAlongNormal;
        impulseScalar /= (1.0f / a.mass + 1.0f / b.mass);

        glm::vec3 impulse = impulseScalar * collisionNormal;
        a.currentVelocity -= (1.0f / a.mass) * impulse;
        b.currentVelocity += (1.0f / b.mass) * impulse;
    }
}



#endif // RIGID_BODY_H