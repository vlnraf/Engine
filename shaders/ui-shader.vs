#version 420 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;
layout (location = 3) in int aTexIndex;

out vec4 OutColor;
out vec2 TexCoord;
flat out int TexIndex;

//uniform mat4 model;
uniform mat4 projection;

void main()
{
    vec4 position = aPos;
    gl_Position = projection * position;
    OutColor = aColor;
    TexCoord = aTexCoord;
    TexIndex = aTexIndex;
}