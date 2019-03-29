#version 440 core
out vec4 outColor;

in vec3 texCoords;

layout(location = 20) uniform samplerCube skybox;

void main(){
	outColor = texture(skybox, texCoords);
}
