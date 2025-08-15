#version 330

out vec4 FragColor;

uniform vec4 PickingColor;

void main(){
    FragColor = PickingColor;
}