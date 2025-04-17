#version 330 core

layout(location = 0) in vec3 aPos;       // Position du sommet
layout(location = 1) in vec2 aTexCoord;  // Coordonnée de texture
layout(location = 2) in vec3 aNormal;    // Normale du sommet

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPosition;

    WorldPos = worldPosition.xyz;
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    TexCoords = aTexCoord;
}
