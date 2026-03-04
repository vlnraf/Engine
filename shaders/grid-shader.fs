#version 330

in vec4 fragColor;
in vec2 fragWorldPos;

out vec4 FragColor;

uniform float cellSize;

void main() {
    vec2 coord = fragWorldPos / cellSize;
    vec2 grid = abs(fract(coord) - 0.5);
    float line = min(grid.x, grid.y);
    float lineWidth = 0.04;
    float alpha = 1.0 - smoothstep(0.0, lineWidth, line);

    vec3 bgColor   = vec3(0.13, 0.13, 0.15);
    vec3 lineColor = vec3(0.25, 0.25, 0.28);

    FragColor = vec4(mix(bgColor, lineColor, alpha), 1.0);
}
