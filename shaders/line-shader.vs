#version 330

// Standard vertex layout - matches all other shaders
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;  // Unused for lines, but present for consistency
layout (location = 2) in vec4 aColor;
layout (location = 3) in int aTexIndex;   // Unused for lines, but present for consistency

out vec4 outColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * aPos;
    outColor = aColor;
    // Note: aTexCoord and aTexIndex are ignored for line rendering
}