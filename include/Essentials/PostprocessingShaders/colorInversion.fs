#version 330

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main(){
    FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoord)), 1.0);
}