#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <iostream>
#include <cstdlib>
#include <float.h>
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


bool rayIntersectsTriangle(const glm::vec3& origin, const glm::vec3& dir,
                           const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                           float& t) {
    const float EPSILON = 1e-6f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);
    if (std::abs(a) < EPSILON) return false;

    float f = 1.0 / a;
    glm::vec3 s = origin - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0) return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0 || u + v > 1.0) return false;

    float tCandidate = f * glm::dot(edge2, q);
    if (tCandidate > EPSILON) {
        t = tCandidate;
        return true;
    }

    return false;
}


bool rayIntersectsMesh(const glm::vec3& origin, const glm::vec3& dir,
                       const Mesh& m, float& tMin, glm::vec3& hitNormal) {
    bool hit = false;
    tMin = FLT_MAX;

    for (int i = 0; i < m.indices.size(); i += 3) {
        glm::vec3 v0 = m.vertices[m.indices[i]];
        glm::vec3 v1 = m.vertices[m.indices[i+1]];
        glm::vec3 v2 = m.vertices[m.indices[i+2]];

        float t;
        if (rayIntersectsTriangle(origin, dir, v0, v1, v2, t)) {
            if (t < tMin) {
                tMin = t;
                hit = true;
                hitNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            }
        }
    }

    return hit;
}





#endif // RIGID_BODY_H