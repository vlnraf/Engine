#version 300 es

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aTexIndex;

out vec4 outColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * aPos;
    outColor = aColor;
}
