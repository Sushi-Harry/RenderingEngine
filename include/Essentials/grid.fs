#version 330 core

in vec3 worldPos;

uniform vec3 CameraPos;
uniform float gridSpacing = 1.0;
uniform float lineThickness = 0.02;

out vec4 FragColor;

void main()
{
    // Calculate distance from the closest X and Z lines
    float lineX = abs(fract(worldPos.x / gridSpacing) - 0.5);
    float lineZ = abs(fract(worldPos.z / gridSpacing) - 0.5);

    // Merge them (draw both X and Z lines)
    float line = min(lineX, lineZ);

    // Control thickness
    float alpha = smoothstep(lineThickness * 0.5, 0.0, line);

    // Fade if too far
    // float dist = length(worldPos.xz - vec2(CameraPos.x, CameraPos.z));
    // alpha *= (1.0 - smoothstep(20.0, 50.0, dist));
    // alpha *= exp(-dist * 0.01);

    float dist = length(worldPos.xz - CameraPos.xz);
    float fade = exp(-dist * 0.4); // Strong fade close to camera
    alpha *= fade;


    vec3 gridColor = vec3(0.2078, 0.2078, 0.2078); // Light gray
    FragColor = vec4(gridColor, alpha);
    
    if (alpha <= 0.01){
        discard;
    }
}
