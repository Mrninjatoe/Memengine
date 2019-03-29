#version 440 core
layout(location = 0) in vec3 pos;

out vec3 texCoords;

layout(location = 0) uniform mat4 v;
layout(location = 1) uniform mat4 p;

void main(){
	texCoords = pos;
	vec4 convertPos = p * v * vec4(pos, 1.f);
	gl_Position = convertPos.xyww;
}
