#version 330

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main(){

    FragColor = texture(screenTexture, TexCoord);
    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;

    FragColor = vec4(vec3(average), 1.0);
}