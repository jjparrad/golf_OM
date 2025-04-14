#version 330 core

in vec2 TexCoord;  // Coordonnées de texture depuis le vertex shader
out vec4 FragColor;

flat in int fragFocus;
flat in int fragUseHeight;

uniform sampler2D texture_earth;




void main()
{
    vec4 color = texture(texture_earth, TexCoord);

    if (fragFocus == 1) {
        color = vec4(1.0, 1.0, 0.2, 1.0);
    }

    FragColor = color;
}