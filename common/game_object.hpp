#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <cstdlib>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "mesh.hpp"
#include "transform.hpp"
#include "rigid_body.hpp"

class GameObject
{
public:
    Transform transform;
    Mesh mesh;
    RigidBody rigidBody;
    Mesh low_mesh;

    bool hasLowMesh = false;
    bool useGravity = true;
    
    std::vector<GameObject*> children;
    GameObject* parent = nullptr;

    // Constructor / Clone
    GameObject();
    GameObject(Mesh& new_mesh) : mesh(new_mesh) {
        rigidBody.transform = &transform;
    };
    
    GameObject clone() const;

    void addChild(GameObject* o) {
        children.push_back(o);
        o->setParent(this);
    };
    
    void setLowMesh(Mesh& new_mesh) {
        low_mesh = new_mesh;
        hasLowMesh = true;
    };

    void setParent(GameObject* o) {
        parent = o;
    };

    glm::mat4 getTransformation() {
        if (parent == nullptr) {
            return transform.getMatrix();
        } else {
            return parent->getTransformation() * transform.getMatrix();
        }
    };

    void translate(const glm::vec3 &translation) {
        transform.translate(translation);
    };

    void scale(const glm::vec3 &scale) {
        transform.applyScale(scale);
    };

    void rotate(float angle, const glm::vec3 &axis) {
        transform.rotate(angle, axis);
    };

    void check() {
        std::cout << "GameObject alive" << std::endl;
    };

    float adjustHeight(GameObject* object) {
        glm::vec3 localPosition = glm::vec3(glm::inverse(transform.getMatrix()) * glm::vec4(object->transform.position, 1.0f));

        float xMin = mesh.minVertex.x;
        float xMax = mesh.maxVertex.x;
        float zMin = mesh.minVertex.z;
        float zMax = mesh.maxVertex.z;

        std::cout << "Object position (global): " << object->transform.position.x << ", " << object->transform.position.y << ", " << object->transform.position.z << std::endl;
        std::cout << "Object position (local to terrain): " << localPosition.x << ", " << localPosition.y << ", " << localPosition.z << std::endl;

        if (localPosition.x < xMin || localPosition.x > xMax ||
            localPosition.z < zMin || localPosition.z > zMax) {
            std::cout << "Not in surface" << std::endl;
            return object->transform.position.y;
        }

        float localHeight = mesh.getHeightInPosition(localPosition);
        float globalHeight = localHeight + transform.position.y;

        return globalHeight;
    };

    void applyGravity(float time) {
        if (useGravity) {
            rigidBody.applyGravity(time);
        }
    };
};




#endif // GAMEOBJECT_H