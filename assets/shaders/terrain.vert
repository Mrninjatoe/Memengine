#version 440 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 4) in vec2 uv;
layout(location = 9) in vec3 offset;

out vec3 vPos;
out vec3 vNormal;
out vec2 vUV;
out vec3 vColor;

layout(location = 0) uniform mat4 v;
layout(location = 1) uniform mat4 p;
layout(location = 2) uniform float scale;
void main(){
	vPos = pos + (offset * scale);
	vNormal = normal;
	vUV = uv;
	vColor = color;
	gl_Position = p * v * vec4(vPos, 1);
}