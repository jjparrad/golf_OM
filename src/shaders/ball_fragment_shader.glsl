#version 330 core

in vec2 TexCoord;  // Coordonnées de texture depuis le vertex shader
out vec4 FragColor;

uniform sampler2D texture_earth;




void main()
{
    vec4 shipColor = texture(texture_earth, TexCoord);
    FragColor = shipColor;
}