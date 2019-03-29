#version 440 core
in vec2 vUV;

out vec4 outColor;

layout(location = 0) uniform mat4 v;
layout(location = 1) uniform mat4 lightViewProjs[4];
layout(location = 5) uniform vec4 cascadedSplits;
layout(location = 6) uniform float zFar;

layout(location = 20) uniform sampler2D positions;
layout(location = 21) uniform sampler2D colors;

uint calculateWhichLayer(){
	vec3 fragWPos = texture(positions, vUV).xyz;
	uint cascadeIndex = 0;
	float positiveViewPosZ = vec3(v * vec4(fragWPos, 1)).z;
	for(uint i = 0; i < 4 - 1; i++){
		if(positiveViewPosZ < cascadedSplits[i]){
			cascadeIndex = i + 1;
		}
	}

	return cascadeIndex;
}

void main(){
	vec3 splitColor = vec3(0);
	uint whichLayer = calculateWhichLayer();

	if(whichLayer == 0)
		splitColor = vec3(0.3,0,0);
	else if(whichLayer == 1)
		splitColor = vec3(0,0.3,0);
	else if(whichLayer == 2)
		splitColor = vec3(0,0,0.3);
	else if(whichLayer == 3)
		splitColor = vec3(0.3, 0, 0.3);
	else if(whichLayer == 4)
		splitColor = vec3(1);

	splitColor *= texture(colors, vUV).rgb;

	outColor = vec4(splitColor, 1);
}