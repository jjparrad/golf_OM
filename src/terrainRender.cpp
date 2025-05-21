#include "common/terrainRender.hpp"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include "common/util.hpp"

void drawObject2(Mesh mesh) {
    glBindVertexArray(mesh.vaoID);
    glDrawElements(GL_TRIANGLES,          // mode
                 mesh.getIndicesSize(), // count
                 GL_UNSIGNED_SHORT,     // type
                 0);              // element array buffer offset
    glBindVertexArray(0);
}



void initTerrainShader(GLuint programID, GLuint textureHeightmap, GLuint texture1) {
    glUseProgram(programID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureHeightmap);
    glUniform1i(glGetUniformLocation(programID, "heightmap"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glUniform1i(glGetUniformLocation(programID, "textureImg"), 1);
}



void renderTerrain(GLuint programID, GameObject* terrain, Camera& camera, bool isFocused) {
    glUseProgram(programID);

    glm::mat4 model = terrain->getTransformation();
    glm::mat4 view = glm::lookAt(camera.position, camera.target + camera.position, camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    glm::mat4 mvp = projection * view * model;

    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniform1i(glGetUniformLocation(programID, "isFocused"), isFocused ? 1 : 0);

    drawObject2(terrain->mesh);
}



void initSkybox(GLuint& vao, GLuint& vbo, GLuint& cubemapTexture, const std::vector<std::string>& faces)
{
    // Charger la cubemap
    cubemapTexture = loadCubemap(faces, true);

    // Initialiser VAO / VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glBindVertexArray(0); // nettoyage
}

void renderSkybox(GLuint vao, GLuint cubemapTexture, GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection)
{
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glUseProgram(shaderProgram);
    
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewNoTranslation));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}