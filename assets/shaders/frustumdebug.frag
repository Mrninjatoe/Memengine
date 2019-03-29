#version 440 core


out vec4 outColor;

layout(location = 10) uniform vec3 frustumColor;

void main(){

	outColor = vec4(frustumColor, 1);
}