#version 440 core

layout(location = 0) in vec3 pos;
layout(location = 4) in vec2 uv;

out vec2 vUV;

void main(){
	vUV = uv;
	gl_Position = vec4(pos, 1);
}