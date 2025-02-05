#version 330 core
in vec4 fragColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(fragColor); // Use the passed color
}