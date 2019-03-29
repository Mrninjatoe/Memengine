#version 440 core
layout(location = 0) in vec3 pos;
layout(location = 4) in vec2 uv;
layout(location = 5) in mat4 modelMatrix;

layout(location = 0) uniform mat4 m;
layout(location = 1) uniform unsigned int instanceCount;

out vec2 vsUV;

void main(){
	mat4 tempM = modelMatrix;
	if(instanceCount == 1)
		tempM = m;
	
	vsUV = uv;
	gl_Position = tempM * vec4(pos, 1);
}