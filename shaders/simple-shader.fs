#version 330
in vec4 fragColor;
in vec2 fragTexCoord;
out vec4 FragColor;

void main() {
    FragColor = vec4(fragColor); // Use the passed color
    // fragTexCoord is available for custom shaders to use
}