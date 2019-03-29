#version 440 core
#extension GL_EXT_texture_array : enable

in GS_OUT{
	vec2 uv;
	flat int layerIndex;
} fs_in;

layout(location = 0) out vec2 blurColor;

layout(location = 1) uniform vec2 direction;
layout(location = 2) uniform float radius;
layout(location = 20) uniform sampler2DArray prevTexture;

void main(){
	float blur = (1.f / textureSize(prevTexture, 0).x) * radius;
	vec2 texCoords = fs_in.uv;
	int layerIndex = fs_in.layerIndex;
	float hStep = direction.x;
	float vStep = direction.y;

	vec2 sum = vec2(0.f);
	sum += texture(prevTexture, vec3(vec2(texCoords.x - 4.0 * blur * hStep, texCoords.y - 4.0 * blur * vStep), layerIndex)).xy * 0.0162162162;
	sum += texture(prevTexture, vec3(vec2(texCoords.x - 3.0 * blur * hStep, texCoords.y - 3.0 * blur * vStep), layerIndex)).xy * 0.0540540541;
	sum += texture(prevTexture, vec3(vec2(texCoords.x - 2.0 * blur * hStep, texCoords.y - 2.0 * blur * vStep), layerIndex)).xy * 0.1216216216;
	sum += texture(prevTexture, vec3(vec2(texCoords.x - 1.0 * blur * hStep, texCoords.y - 1.0 * blur * vStep), layerIndex)).xy * 0.1945945946;
	
	sum += texture(prevTexture, vec3(texCoords.xy, layerIndex)).xy * 0.2270270270;
	
	sum += texture(prevTexture, vec3(vec2(texCoords.x + 1.0 * blur * hStep, texCoords.y + 1.0 * blur * vStep), layerIndex)).xy * 0.1945945946;
	sum += texture(prevTexture, vec3(vec2(texCoords.x + 2.0 * blur * hStep, texCoords.y + 2.0 * blur * vStep), layerIndex)).xy * 0.1216216216;
	sum += texture(prevTexture, vec3(vec2(texCoords.x + 3.0 * blur * hStep, texCoords.y + 3.0 * blur * vStep), layerIndex)).xy * 0.0540540541;
	sum += texture(prevTexture, vec3(vec2(texCoords.x + 4.0 * blur * hStep, texCoords.y + 4.0 * blur * vStep), layerIndex)).xy * 0.0162162162;

	blurColor = sum;
}