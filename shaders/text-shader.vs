#version 420 core
layout (location = 0) in vec2 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec2 texCoord;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex, 1.0, 1.0); 
    TexCoords = texCoord;
} 