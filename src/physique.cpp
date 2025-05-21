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