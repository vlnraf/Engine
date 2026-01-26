#version 300 es
precision mediump float;

in vec4 fragColor;
in vec2 fragTexCoord;
out vec4 FragColor;

uniform float dt;

void main() {
    vec2 uv = fragTexCoord;

    // === EFFECT 6: Plasma Effect ===
    float plasma = sin(uv.x * 10.0 + dt) + sin(uv.y * 10.0 + dt * 1.3) +
                    sin((uv.x + uv.y) * 10.0 + dt * 0.7) + sin(length(uv - 0.5) * 20.0 + dt * 2.0);
    vec3 color = vec3(sin(plasma) * 0.5 + 0.5, cos(plasma * 1.5) * 0.5 + 0.5, sin(plasma * 2.0) * 0.5 + 0.5);

    FragColor.rgb = fragColor.rgb * color;
    FragColor.a = 1.0;
}
