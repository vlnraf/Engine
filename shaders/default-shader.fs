#version 330 core

in vec4 vertexColor;
in vec2 TexCoord;

uniform sampler2D sprite;

out vec4 FragColor;

void main()
{
    FragColor = vertexColor;
    FragColor = texture(sprite, TexCoord);
}