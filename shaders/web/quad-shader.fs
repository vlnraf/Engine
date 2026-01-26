#version 300 es
precision mediump float;

in vec4 OutColor;
in vec2 TexCoord;
flat in int TexIndex;

uniform sampler2D sprite[16];

out vec4 FragColor;

void main()
{
    // WebGL doesn't support dynamic sampler array indexing
    // Use ternary chain instead
    FragColor = (TexIndex == 0 ? texture(sprite[0], TexCoord) :
                 TexIndex == 1 ? texture(sprite[1], TexCoord) :
                 TexIndex == 2 ? texture(sprite[2], TexCoord) :
                 TexIndex == 3 ? texture(sprite[3], TexCoord) :
                 TexIndex == 4 ? texture(sprite[4], TexCoord) :
                 TexIndex == 5 ? texture(sprite[5], TexCoord) :
                 TexIndex == 6 ? texture(sprite[6], TexCoord) :
                 TexIndex == 7 ? texture(sprite[7], TexCoord) :
                 TexIndex == 8 ? texture(sprite[8], TexCoord) :
                 TexIndex == 9 ? texture(sprite[9], TexCoord) :
                 TexIndex == 10 ? texture(sprite[10], TexCoord) :
                 TexIndex == 11 ? texture(sprite[11], TexCoord) :
                 TexIndex == 12 ? texture(sprite[12], TexCoord) :
                 TexIndex == 13 ? texture(sprite[13], TexCoord) :
                 TexIndex == 14 ? texture(sprite[14], TexCoord) :
                 texture(sprite[15], TexCoord)) * OutColor;
    if (FragColor.a <= 0.1) {
        discard;
    }
}
