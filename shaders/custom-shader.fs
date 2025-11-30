#version 330
in vec4 fragColor;
in vec2 fragTexCoord;
out vec4 FragColor;

uniform float dt;

void main() {
    vec2 uv = fragTexCoord;

    // === EFFECT 1: UV Gradient ===
    // Creates a gradient from bottom-left to top-right
    //vec3 color = vec3(uv.x, uv.y, 0.5);

    // === EFFECT 2: Radial Rainbow ===
    // Distance from center creates circular rainbow
    //vec2 center = vec2(0.5, 0.5);
    //float dist = distance(uv, center);
    //vec3 color;
    //color.r = sin(dist * 10.0 + dt * 2.0) * 0.5 + 0.5;
    //color.g = sin(dist * 10.0 + dt * 2.0 + 2.0) * 0.5 + 0.5;
    //color.b = sin(dist * 10.0 + dt * 2.0 + 4.0) * 0.5 + 0.5;

    // === EFFECT 3: Wave Distortion ===
    //float wave = sin(uv.x * 10.0 + dt) * sin(uv.y * 10.0 + dt);
    //vec3 color = vec3(wave * 0.5 + 0.5);

    // === EFFECT 4: Rotating Pattern ===
    //vec2 centered = uv - 0.5;
    //float angle = atan(centered.y, centered.x) + dt;
    //vec3 color = vec3(sin(angle * 3.0) * 0.5 + 0.5, cos(angle * 3.0) * 0.5 + 0.5, 0.5);

    // === EFFECT 5: Checkerboard with UV ===
    //float checker = mod(floor(uv.x * 8.0) + floor(uv.y * 8.0), 2.0);
    //vec3 color = mix(vec3(0.2), vec3(0.8), checker);

    // === EFFECT 6: Plasma Effect ===
    float plasma = sin(uv.x * 10.0 + dt) + sin(uv.y * 10.0 + dt * 1.3) +
                    sin((uv.x + uv.y) * 10.0 + dt * 0.7) + sin(length(uv - 0.5) * 20.0 + dt * 2.0);
    vec3 color = vec3(sin(plasma) * 0.5 + 0.5, cos(plasma * 1.5) * 0.5 + 0.5, sin(plasma * 2.0) * 0.5 + 0.5);

    //vec2 center = vec2(0.5, 0.5);
    //vec2 centered = uv - center;
    //float dist = distance(uv, center);
    //float radius = 0.5;

    //// Discard pixels outside the circle
    //if(dist > radius){
    //    discard;
    //}

    //// === SPHERE CALCULATION ===
    //// Calculate the z-depth to create sphere illusion
    //float z = sqrt(max(0.0, radius * radius - dist * dist));
    //float normalizedZ = z / radius; // 0 at edges, 1 at center

    //// Calculate surface normal for lighting
    //vec3 normal = normalize(vec3(centered, z));

    //// Light direction (from top-right-front)
    //vec3 lightDir = normalize(vec3(0.5, 0.5, 1.0));

    //// Calculate diffuse lighting
    //float diffuse = max(dot(normal, lightDir), 0.0);

    //// Add specular highlight
    //vec3 viewDir = vec3(0.0, 0.0, 1.0);
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    //// Ambient occlusion at edges
    //float ao = smoothstep(0.0, 0.3, normalizedZ);

    //// === BOUNCY BALL SHADING ===

    //// Base ball color (you can change this)
    //vec3 ballColor = vec3(1.0, 0.3, 0.2); // Orange-red ball

    //// Lighting parameters
    //float ambientStrength = 0.2;   // Ambient light level
    //float diffuseStrength = 0.8;   // Diffuse light strength
    //float specularStrength = 0.8;  // Specular highlight intensity
    //float shininess = 64.0;        // Specular shininess (higher = smaller, sharper)

    //// Recalculate specular with custom shininess
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    //// Apply diffuse lighting (Lambertian shading)
    //float lighting = diffuse * diffuseStrength + ambientStrength;

    //// Apply lighting to ball color
    //vec3 color = ballColor * lighting;

    //// Add specular highlight
    //color += vec3(1.0) * spec * specularStrength;

    FragColor.rgb = fragColor.rgb * color;
    FragColor.a = 1.0;
}