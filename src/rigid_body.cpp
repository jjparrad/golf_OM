#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <iostream>
#include <cstdlib>
#include <float.h>
#include "common/rigid_body.hpp"
#include "common/transform.hpp"



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

void RigidBody::applySlopeForce(float dt, const glm::vec3& N) {
    const glm::vec3 G = glm::vec3(0.0f, -9.82f, 0.0f);
    glm::vec3 slopeForce = G - glm::dot(G, N) * N;
    // on scale un peu pour sentir le roulement
    float slopeStrength = 1.5f;
    currentVelocity += slopeForce * dt * slopeStrength;
}


bool areSpheresColliding(const Transform& a, const Transform& b, float radiusA, float radiusB) {
    float distance = glm::distance(a.position, b.position);
    return distance < (radiusA + radiusB);
}







#endif // RIGID_BODY_H