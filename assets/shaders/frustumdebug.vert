#version 440 core

in layout(location = 0) vec3 pos;

layout(location = 0) uniform mat4 m;
layout(location = 1) uniform mat4 vp;

void main(){
	
	gl_Position = vp * m * vec4(pos, 1.f);
}