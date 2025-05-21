#version 330 core

in vec2 textureCoords;
in float height;
flat in int fragFocus;
flat in int fragUseHeight;

uniform sampler2D textureImg;


// Ouput data
out vec4 color;

void main() {

    color = texture(textureImg, textureCoords);
    

    if (fragFocus == 1) {
        color = vec4(1.0, 1.0, 0.2, 1.0);
    }
    // color = texture(heightmap, textureCoords);
    // color = vec4(textureCoords, 1.0, 1.0);
}

