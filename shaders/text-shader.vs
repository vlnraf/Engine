#version 330

// Standard vertex layout - matches all other shaders
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;     // Unused for text, but present for consistency
layout (location = 3) in int aTexIndex;   // Unused for text, but present for consistency

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * aPos;
    TexCoords = aTexCoord;
    // Note: aColor and aTexIndex are ignored for text rendering
} 