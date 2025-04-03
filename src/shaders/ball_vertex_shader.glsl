#version 330 core

layout(location = 0) in vec3 aPos;       // Position attribute
layout(location = 1) in vec2 aTexCoord; // Texture coordinate attribute

out vec2 TexCoord;                      // Output to fragment shader
uniform mat4 MVP;


void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);      // Pass the position to gl_Position
    TexCoord = aTexCoord;          // Pass the texture coordinate
}