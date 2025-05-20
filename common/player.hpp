#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <cstdlib>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "game_object.hpp"
class Player {
public:
    GameObject* gameObject = nullptr;
    bool wasMoving = false;

    void setPlayer(GameObject* go) {
        if (!go->isPlayer) {
            return;
        }
        gameObject = go;
        wasMoving = false;
    };

    void saveCameraDirection(glm::vec3 direction) {
        if (!gameObject) {
            return;
        }
        gameObject->cameraDirection = direction;
    };

    bool finishTurn() {
        if (!gameObject) {
            return false;
        }

        bool isMoving = gameObject->rigidBody.ismoving;
        if (isMoving) {
            wasMoving = true;
            return false;
        }

        if (wasMoving && !isMoving) {
            return true;
        }
        
    };
};

#endif // PLAYER_H