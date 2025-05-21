#include "common/physique.hpp"




void processPhysique(GameObject* current,GameObject* terrain,float deltaTime ){
        float minBounceVelocity = 0.3f;
        float sphereRadius = 0.06f;
        float restitutionFactor = 0.55f;
        float rollFriction = 0.4f;

        RigidBody& rb = current->rigidBody;
        Transform& tf = current->transform;

        if (current->usePhysics) {
            rb.applyGravity(deltaTime);
            rb.physicsLoop(deltaTime);
        }

        // Stop object if it falls too low
        if (tf.position.y < -4.0f) {
            rb.currentVelocity = glm::vec3(0.0f);
            tf.setPosition(current->lastPlayerspos);
        }

        // Collision avec le sol (raycast vers le bas)
        float tHit;
        glm::vec3 groundNormal;
        bool hitGround = rayIntersectsMesh(tf.position, glm::vec3(0.0f, -1.0f, 0.0f), terrain->mesh, tHit, groundNormal);

        if (hitGround) {
            float contactY = tf.position.y - tHit;
            float penetration = (contactY + sphereRadius) - tf.position.y;

            if (penetration > 0.0f) {
                // Correction de la position
                tf.position.y += penetration * 1.5f;

                float speed = glm::length(rb.currentVelocity);
                if (speed < 0.2f) {
                    current->lastPlayerspos = tf.position + glm::vec3(0.0f, 1.0f, 0.0f);
                    rb.ismoving = false;
                }

                // Rebond
                if (rb.currentVelocity.y < 0.0f)
                    rb.currentVelocity.y *= -restitutionFactor;

                rb.stopGravity();
            }

            // Appliquer la friction si on est sur une pente
            rb.applySlopeForce(deltaTime, groundNormal);

            float speed = glm::length(rb.currentVelocity);
            if (speed > 1e-4f) {
                glm::vec3 frictionDir = -glm::normalize(rb.currentVelocity);
                glm::vec3 friction = frictionDir * rollFriction * deltaTime;

                if (glm::length(friction) > speed)
                    friction = -rb.currentVelocity;

                rb.currentVelocity += friction;
            } else {
                rb.currentVelocity = glm::vec3(0.0f);
            }
        }

        // Détection collision avec mesh (rebond)
       glm::vec3 velocity = rb.currentVelocity;
        float velLen = glm::length(velocity);
        if (velLen > 1e-4f) {
            glm::vec3 direction = glm::normalize(velocity);
            float maxDist = velLen * deltaTime;
            float t; glm::vec3 normal;
            if (rayIntersectsMesh(tf.position, direction, terrain->mesh, t, normal) && t < maxDist) {
                tf.position += direction * t;

                glm::vec3 reflected = glm::reflect(velocity, normal) * restitutionFactor;
                if (glm::length(reflected) < minBounceVelocity) {
                    reflected = glm::vec3(0.0f);
                    rb.stopGravity();
                    rb.slowDown(deltaTime);
                }
                rb.currentVelocity = reflected;
            }
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





void spheresCollision( std::vector<GameObject*> gameObjects, float sphereRadius){
    for (int a = 0; a < gameObjects.size(); ++a) {
            for (int b = a + 1; b < gameObjects.size(); ++b) {
                if (a == 0 || b == 0) continue; // ignorer le terrain

                Transform& ta = gameObjects[a]->transform;
                Transform& tb = gameObjects[b]->transform;
                RigidBody& ra = gameObjects[a]->rigidBody;
                RigidBody& rb = gameObjects[b]->rigidBody;

                if (areSpheresColliding(ta, tb, sphereRadius / 2, sphereRadius / 2)) {
                    resolveSphereCollision(ra, rb, sphereRadius / 2, sphereRadius / 2);
                }
            }
        }
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


