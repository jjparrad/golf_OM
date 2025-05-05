#pragma once

#include <GL/gl.h>
#include <glm/geometric.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Light{
    public:
        glm::vec3 pos;
        glm::vec3 color;

    Light() = default;
    Light(glm::vec3 new_pos, glm::vec3 new_color){
        pos = new_pos;
        color = new_color;
    }
};