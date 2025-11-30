#version 330
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    // Font texture is GL_RED format
    // On desktop with texture swizzle: returns (1,1,1,r)
    // On WebGL without swizzle: returns (r,0,0,1)
    // Use only the red channel as alpha for both cases
    vec4 sampled = texture(text, TexCoords);
    color = vec4(textColor, sampled.r);
}  