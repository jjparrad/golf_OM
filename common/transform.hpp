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
    glm::mat4 m;
    glm::vec3 position;
    glm::vec3 size;

    Transform() {
        m = glm::mat4(1);
        position = glm::vec3(0.0, 0.0, 0.0);
    };

    void scale(const glm::vec3 &scale) {
        size = scale;
        m = glm::scale(m, scale);
    };

    void translate(const glm::vec3 &translation) {
        position += translation;
        m = glm::translate(m, translation);
    };

    void rotate(float angle, const glm::vec3 &axis) {
        m = glm::rotate(m, angle, axis);
    };

    glm::vec4 apply(glm::vec4 p);

    glm::vec3 applyToPoint(glm::vec3 p) {
        glm::vec4 p4(p, 1.0f);
        glm::vec4 transformed = m * p4;
        return glm::vec3(transformed);
    }
    
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