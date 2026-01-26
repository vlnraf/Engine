#version 300 es
precision mediump float;

in vec4 fragColor;
in vec2 fragTexCoord;
out vec4 FragColor;

void main() {
    FragColor = vec4(fragColor);
}
