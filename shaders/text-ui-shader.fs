#version 420 core

in vec4 OutColor;
in vec2 TexCoords;

out vec4 color;

uniform sampler2D text;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    //color = texture(text, TexCoords) * OutColor;
    color = OutColor * sampled;
}  