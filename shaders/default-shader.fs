#version 330 core

in vec4 vertexColor;
in vec2 TexCoord;

uniform sampler2D sprite;

out vec4 FragColor;

void main()
{
    FragColor = vertexColor;
    FragColor = texture(sprite, TexCoord);
    if (FragColor.a <= 0.9) {
        discard; // Discards the fragment if alpha is less than or equal to 0.9
    }
}