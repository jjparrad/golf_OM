#version 330 core

in vec2 textureCoords;
in float height;
flat in int fragFocus;
flat in int fragUseHeight;

uniform sampler2D textureImgLow;
uniform sampler2D textureImgMid;
uniform sampler2D textureImgHigh;
uniform sampler2D heightmap;

// Ouput data
out vec4 color;

void main() {

    vec4 grass = texture(textureImgLow, textureCoords);
    vec4 rock = texture(textureImgMid, textureCoords);
    vec4 snow = texture(textureImgHigh, textureCoords);


    float grassHeight = smoothstep(0.0, 0.3, height);
    float rockHeight = smoothstep(0.3, 0.4, height);
    float snowHeight = smoothstep(0.4, 1.0, height); 

    vec4 grassRock = mix(grass, rock, grassHeight);
    vec4 rockSnow = mix(rock, snow, rockHeight);
    color = mix(grassRock, rockSnow, snowHeight);
    

    if (fragFocus == 1) {
        color = vec4(1.0, 1.0, 0.2, 1.0);
    }
    // color = texture(heightmap, textureCoords);
    // color = vec4(textureCoords, 1.0, 1.0);
}

