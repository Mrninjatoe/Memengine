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
layout(location = 2) uniform float numberOfTiles;
layout(location = 3) uniform float tileSize;

layout(location = 20) uniform sampler2D lilTex;

void main(){
	float tilesPerAxis = sqrt(numberOfTiles);
	vPos = (pos * tileSize) + (offset * numberOfTiles * tileSize);
	vNormal = normal;
	vec2 newUV = vPos.xz / (tilesPerAxis * tileSize); // not sure lamo
	float height = texture(lilTex, newUV).r;
	vPos.y = height * tileSize * 10;
	vUV = newUV;
	vColor = color;
	gl_Position = p * v * vec4(vPos, 1);
}