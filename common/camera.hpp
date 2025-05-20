#ifndef CAMERA_H
#define CAMERA_H

#include <glm/geometric.hpp>
#include <iostream>
#include <cstdlib>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "game_object.hpp"
class Camera {
public:
    glm::vec3 target = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));
    glm::vec3 position = glm::vec3(0.0f, 5.0f, 5.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    
    void setTarget(GameObject* newCameraTarget) {
        cameraTarget = newCameraTarget;
        updateOffset();
    };

    void orbitalRotation(int direction) {
        if (!cameraTarget) {
            return;
        }
        float angle = glm::radians(rotationSpeed) * static_cast<float>(direction);

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
        offsetFromTarget = glm::vec3(rotation * glm::vec4(offsetFromTarget, 0.0f));
    };

    void updatePosition() {
        if (!cameraTarget) return;
        position = cameraTarget->transform.position + offsetFromTarget;
        target = glm::normalize(cameraTarget->transform.position - position);
    }

private:
    GameObject* cameraTarget = nullptr;

    float rotationSpeed = 1.0f;
    float distanceToObject = 2.0f;
    float cameraHeight = 1.0f;
    glm::vec3 offsetFromTarget = glm::vec3(0.0f, -2.0f, 0.0f);

    void updateOffset() {
        if (!cameraTarget) return;
        
        position = cameraTarget->transform.position + offsetFromTarget;
        
        glm::vec3 direction = cameraTarget->cameraDirection * glm::vec3(-1, 1, -1);
        direction.y = 0;
        glm::vec3 normalizedDirection = glm::normalize(direction);
        normalizedDirection.y = cameraHeight;
        offsetFromTarget = normalizedDirection * distanceToObject;
    }
};

#endif // CAMERA_H