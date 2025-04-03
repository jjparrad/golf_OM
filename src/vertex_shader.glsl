#version 330 core

layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec2 aTexCoords;

// Model View Projection matrix
uniform mat4 MVP;
uniform sampler2D heightmap;

uniform int useHeight;
uniform int isFocused;

out vec2 textureCoords;
out float height;
flat out int fragUseHeight;
flat out int fragFocus;

void main() {
        if(useHeight == 1) {
                float h = texture(heightmap, aTexCoords).r;
                // vec3 newPosition = vec3(vertices_position_modelspace.x, h, vertices_position_modelspace.z);
                vec3 newPosition = vertices_position_modelspace;
                gl_Position = MVP * vec4(newPosition, 1.0);
                
                height = h;
        } else {
                gl_Position = MVP * vec4(vertices_position_modelspace, 1.0);
                height = vertices_position_modelspace.y;
        }
        
        textureCoords = aTexCoords;
        fragUseHeight = useHeight;
        fragFocus = isFocused;
}

