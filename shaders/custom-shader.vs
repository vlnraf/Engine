#version 330

// Standard vertex layout - matches all other shaders
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;
layout (location = 3) in int aTexIndex;   // Unused for custom shader, but present for consistency

out vec4 fragColor;
out vec2 fragTexCoord;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * aPos;
    fragColor = aColor;
    fragTexCoord = aTexCoord;  // Pass UV coordinates for fancy effects
    // Note: aTexIndex is ignored for this custom shader
}