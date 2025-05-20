
#ifndef TEXT_RENDER_HPP
#define TEXT_RENDER_HPP


#include <GL/glew.h>    
#include <GLFW/glfw3.h>    
#include <glm/glm.hpp>      
#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H



struct Character {
    unsigned int TextureID;  
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};



void InitTextRendering(const std::string& fontPath);

void RenderText(GLuint s, std::string text, float x, float y, float scale, glm::vec3 color);


#endif // TEXT_RENDER_HPP