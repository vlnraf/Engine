#version 330 core

layout (location = 0) in vec3 aPos;

out vec4 vertexColor;

uniform mat4 transform;
uniform mat4 projection;

void main()
{
    gl_Position = projection * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vertexColor = vec4(0.7, 0.3, 0.5, 1.0);
}