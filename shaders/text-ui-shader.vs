#version 420 core
//layout (location = 0) in vec2 vertex; // <vec2 pos, vec2 tex>
//layout (location = 1) in vec2 texCoord;
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;
layout (location = 3) in int aTexIndex;

out vec2 TexCoords;
out vec4 OutColor;

uniform mat4 projection;

void main()
{
    gl_Position = projection * aPos; 
    OutColor = aColor;
    TexCoords = aTexCoord;
} 