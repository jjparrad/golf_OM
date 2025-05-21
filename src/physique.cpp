#include "common/physique.hpp"
#include "common/util.hpp"

#include <math.h>




void processPhysique(GameObject* current,GameObject* terrain,float deltaTime ){
        float minBounceVelocity = 0.1f;
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
        bool hitGround = (rayIntersectsMesh(tf.position, glm::vec3(0.0f, -1.0f, 0.0f), terrain->mesh, tHit, groundNormal) && tHit < 0.1);


        glm::vec3 velocity = rb.currentVelocity;
        glm::vec3 endPos = tf.position + velocity * deltaTime;
        float t;
        glm::vec3 normal;
        bool hitMesh = sphereCast(tf.position, endPos, sphereRadius/2, terrain->mesh, t, normal);

        if (hitGround) {
            //printf("collision detectée sol \n");
            float contactY = tf.position.y - tHit;
            float penetration = (contactY + sphereRadius) - tf.position.y;

            if (penetration > 0.0f) {
                // Correction de la position
                tf.position += groundNormal * penetration * 1.5f;

                float speed = glm::length(rb.currentVelocity);
                if (speed < 0.2f) {
                    current->lastPlayerspos = tf.position + glm::vec3(0.0f, 1.0f, 0.0f);
                    rb.ismoving = false;
                }

                 //Rebond
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
        
        float velLen = glm::length(velocity);

        if (velLen > 1e-4f) {

            if (hitMesh) {
                printf("collision detectée mesh \n");


                // Déplacement jusqu'au point de contact
                tf.position += velocity * deltaTime * t;

                // Repousser légèrement hors du mur (évite encastrement)
                const float pushOut = 0.01f;
                tf.position += normal * pushOut;

                // Vérifier que la vitesse va bien vers la surface
                if (glm::dot(velocity, normal) < 0.0f) {
                    glm::vec3 reflected = glm::reflect(velocity, normal) * restitutionFactor;

                    // Atténuation des faibles rebonds
                    if (glm::length(reflected) < minBounceVelocity) {
                        reflected = glm::vec3(0.0f);
                        rb.stopGravity();
                        rb.slowDown(deltaTime);
                    }

                    rb.currentVelocity = reflected;
                }
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


 //========================================================================
 // deuxième méthode

float closestDistanceSegmentTriangle(const glm::vec3& segA, const glm::vec3& segB,
                                     const glm::vec3& triA, const glm::vec3& triB, const glm::vec3& triC,
                                     glm::vec3& outNormal, float& outT) {
    glm::vec3 u = segB - segA;
    glm::vec3 normal = glm::normalize(glm::cross(triB - triA, triC - triA));
    
    // Approximate t using projection on movement direction
    glm::vec3 toTri = triA - segA;
    float proj = glm::dot(toTri, glm::normalize(u));
    float segLen = glm::length(u);
    outT = glm::clamp(proj / segLen, 0.0f, 1.0f);
    outNormal = normal;

    // Sample point along segment
    glm::vec3 p = segA + u * outT;

    // Find closest point on triangle
    glm::vec3 q = closestPointOnTriangle(p, triA, triB, triC);

    return glm::length(p - q);
}



bool sphereCast(const glm::vec3& start, const glm::vec3& end, float radius,
                const Mesh& mesh, float& tHit, glm::vec3& hitNormal) {
    bool hit = false;
    float closestT = 1.0f;

    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        glm::vec3 v0 = mesh.vertices[mesh.indices[i]];
        glm::vec3 v1 = mesh.vertices[mesh.indices[i + 1]];
        glm::vec3 v2 = mesh.vertices[mesh.indices[i + 2]];

        glm::vec3 normal;
        float t;
        float dist = closestDistanceSegmentTriangle(start, end, v0, v1, v2, normal, t);

        if (dist < radius && t < closestT) {
            hit = true;
            closestT = t;
            hitNormal = normal;
        }
    }

    if (hit)
        tHit = closestT;
    return hit;
}





glm::vec3 closestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ap = p - a;

    float d1 = glm::dot(ab, ap);
    float d2 = glm::dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a;

    glm::vec3 bp = p - b;
    float d3 = glm::dot(ab, bp);
    float d4 = glm::dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return b;

    glm::vec3 cp = p - c;
    float d5 = glm::dot(ab, cp);
    float d6 = glm::dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c;

    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        return a + v * ab;
    }

    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float v = d2 / (d2 - d6);
        return a + v * ac;
    }

    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + v * (c - b);
    }

    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + v * ab + w * ac;
}



