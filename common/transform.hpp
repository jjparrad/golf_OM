#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <iostream>
#include <cstdlib>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Transform {
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::mat4 rotation;

    Transform() {
        position = glm::vec3(0.0);
        scale = glm::vec3(1.0);
        rotation = glm::mat4(1.0);
    };

    glm::mat4 getMatrix() {
        glm::mat4 m(1);
        m = glm::translate(m, position);
        m = glm::scale(m, scale);

        return m;
    };

    void applyScale(const glm::vec3 &newScale) {
        scale *= newScale;
    };

    void translate(const glm::vec3 &translation) {
        position += translation;
    };

    void setYPosition(float y) {
        glm::vec3 newPosition = glm::vec3( position.x, y, position.z);
        setPosition(newPosition);
    };

    void setPosition(const glm::vec3 &newPosition) {
        position = newPosition;
    };

    void rotate(float angle, const glm::vec3 &axis) {
        rotation = glm::rotate(rotation, angle, axis);
    };

    glm::vec4 apply(glm::vec4 p);

    glm::vec3 applyToPoint(glm::vec3 p);
    glm::vec3 applyToVector(glm::vec3 v);
    glm::vec3 applyToVersor(glm::vec3 v);

    Transform combine_with(Transform & t);
    Transform inverse();
    Transform interpolate_with(Transform & t, float k);

    void check() {
        std::cout << "Transform alive" << std::endl;
    };  
};

#endif // TRANSFORM_H