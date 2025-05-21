#include "common/PBRrender.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/physique.hpp"
#include "common/camera.hpp"


static GLuint mvp_uniform, light_pos_uniform, light_col_uniform;
static GLuint cam_pos_uniform, modelLoc, viewLoc, projLoc;
static GLuint metal_f_uniform, rough_f_uniform, ao_uniform, albedo_uniform;


void drawObject3(Mesh mesh) {
    glBindVertexArray(mesh.vaoID);
    glDrawElements(GL_TRIANGLES,          // mode
                 mesh.getIndicesSize(), // count
                 GL_UNSIGNED_SHORT,     // type
                 0);              // element array buffer offset
    glBindVertexArray(0);
}




void initPRB(GLuint program) {
    glUseProgram(program);

    // Uniform locations
    mvp_uniform = glGetUniformLocation(program, "MVP");
    light_pos_uniform = glGetUniformLocation(program, "lightPositions");
    light_col_uniform = glGetUniformLocation(program, "lightColors");
    cam_pos_uniform = glGetUniformLocation(program, "camPos");
    modelLoc = glGetUniformLocation(program, "model");
    viewLoc = glGetUniformLocation(program, "view");
    projLoc = glGetUniformLocation(program, "projection");

    metal_f_uniform = glGetUniformLocation(program, "metallic");
    rough_f_uniform = glGetUniformLocation(program, "roughness");
    ao_uniform = glGetUniformLocation(program, "ao");
    albedo_uniform = glGetUniformLocation(program, "albedo");
}

void renderPRB(GLuint program,
               const std::vector<Light>& lights,
               Camera& camera,
               std::vector<GameObject*>& gameObjects,
               float deltaTime) {

    glUseProgram(program);


    std::vector<glm::vec3> lightPositions, lightColors;
    for (const auto& light : lights) {
        lightPositions.push_back(light.pos);
        lightColors.push_back(light.color);
    }

    glUniform3fv(light_pos_uniform, lights.size(), &lightPositions[0][0]);
    glUniform3fv(light_col_uniform, lights.size(), &lightColors[0][0]);
    glUniform3f(cam_pos_uniform, camera.position[0], camera.position[1], camera.position[2]);

    for (size_t i = 1; i < gameObjects.size(); i++) {
        float minBounceVelocity = 0.3f;
        float sphereRadius = 0.06f;
        float restitutionFactor = 0.55f;
        float rollFriction = 0.4f;

        processPhysique(gameObjects[i], gameObjects[0], deltaTime);
        spheresCollision(gameObjects, sphereRadius);

        glm::mat4 model = gameObjects[i]->getTransformation();
        glm::mat4 view = glm::lookAt(camera.position, camera.position + camera.target, camera.up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

        Material currentMat = gameObjects[i]->mesh.material;
        glUniform1f(metal_f_uniform, currentMat.metal_f);
        glUniform1f(rough_f_uniform, currentMat.rough_f);
        glUniform1f(ao_uniform, currentMat.ao);
        glUniform3f(albedo_uniform, currentMat.albedo[0], currentMat.albedo[1], currentMat.albedo[2]);

        drawObject3(gameObjects[i]->mesh);
    }
}