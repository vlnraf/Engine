#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

out vec4 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 position = model * vec4(aPos, 1.0);
    gl_Position = projection * view * position;
    fragColor = aColor;
}