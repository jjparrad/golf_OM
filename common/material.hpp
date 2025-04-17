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

class Material {
    public:
        glm::vec3 albedo;
        float metal_f;
        float rough_f;
        float ao;


        Material() = default;
        Material(glm::vec3 new_albedo, float new_metal_f, float new_rough_f, float new_ao){
            albedo = new_albedo;
            metal_f = new_metal_f;
            rough_f = new_rough_f;
            ao = new_ao;
        }

};