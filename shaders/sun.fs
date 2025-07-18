#version 330

out vec4 FragColor;
in vec2 TexCoords;

struct Material{
    sampler2D texture_diffuse1;
};

uniform Material material;

void main(){
    // FragColor = vec4(lightColor, 1.0);
    FragColor = texture(material.texture_diffuse1, TexCoords);
}