#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec4 OutColor;
out vec2 TexCoord;

in vec4 inColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 position = model * vec4(aPos, 1.0);
    gl_Position = projection * view * position;
    OutColor = inColor;
    TexCoord = aTexCoord;
}