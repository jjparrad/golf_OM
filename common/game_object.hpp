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
            return transform.m;
        } else {
            return parent->getTransformation() * transform.m;
        }
    };

    void translate(const glm::vec3 &translation) {
        transform.translate(translation);
    };

    void scale(const glm::vec3 &scale) {
        transform.scale(scale);
    };

    void rotate(float angle, const glm::vec3 &axis) {
        transform.rotate(angle, axis);
    };

    void check() {
        std::cout << "GameObject alive" << std::endl;
    };
    
    float adjustHeight(GameObject* object) {
        glm::vec3 position = (object->transform.position - transform.position);

        if (position[0] > mesh.maxVertex[0] || position[1] > mesh.maxVertex[1] || position[2] > mesh.maxVertex[2]) {
            // std::cout << "Outiside or over" << std::endl;
            return 0;
        }

        if (position[0] < mesh.minVertex[0] || position[2] < mesh.minVertex[2]) {
            // std::cout << "Outside" << std::endl;
            return 0;
        }

        float xMin = mesh.minVertex[0];
        float xMax = mesh.maxVertex[0];
        
        float zMin = mesh.minVertex[2];
        float zMax = mesh.maxVertex[2];

        float transformedHeight = mesh.getHeightInPosition(position);

        if (transformedHeight > position[1]) {
            return position[1] - transformedHeight;
        } else {
            return 0;
        }
    };
};




#endif // GAMEOBJECT_H