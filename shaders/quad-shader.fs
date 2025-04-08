#version 420 core

in vec4 OutColor;
in vec2 TexCoord;
flat in int TexIndex;

layout(binding = 0) uniform sampler2D sprite[16];

out vec4 FragColor;

void main()
{
    FragColor = texture(sprite[TexIndex], TexCoord) * OutColor;
    if (FragColor.a <= 0.1) {
        //NOTE: was it necessary for the ysorting???
        discard; // Discards the fragment if alpha is less than or equal to 0.9
    }
}