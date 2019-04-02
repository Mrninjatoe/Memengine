#version 440 core

out vec4 outColor;

in vec2 vUV;

layout(location = 15) uniform float fxaaSpanMax;
layout(location = 16) uniform float fxaaReduceMin;
layout(location = 17) uniform float fxaaReduceMul;
layout(location = 18) uniform vec2 textureSizes;

layout(location = 20) uniform sampler2D sceneColors;
layout(location = 21) uniform sampler2D sceneDepth;

const vec3 luma = vec3(0.299f, 0.587f, 0.114f);

void main(){
	vec2 texOffset = textureSizes;
	float lumaTL = dot(luma, texture(sceneColors, vUV + (vec2(-1.0, -1.0) * texOffset)).xyz);
	float lumaTR = dot(luma, texture(sceneColors, vUV + (vec2(1.0, -1.0) * texOffset)).xyz);
	float lumaBL = dot(luma, texture(sceneColors, vUV + (vec2(-1.0, 1.0) * texOffset)).xyz);
	float lumaBR = dot(luma, texture(sceneColors, vUV + (vec2(1.0, 1.0) * texOffset)).xyz);
	float lumaM = dot(luma, texture(sceneColors, vUV).xyz);

	vec2 blurDir;
	blurDir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	blurDir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (fxaaReduceMul * 0.25), fxaaReduceMin); // Reducing the scaling so we don't divide by 0.
	float inverseAdjustment = 1.0 / (min(abs(blurDir.x), abs(blurDir.y)) + dirReduce);

	blurDir = min(vec2(fxaaSpanMax, fxaaSpanMax), 
	max(vec2(-fxaaSpanMax, -fxaaSpanMax), blurDir * inverseAdjustment)) * texOffset; // Clamping and turn it into texel space.

	vec3 result1 = (1.0/2.0) * (
		texture(sceneColors, vUV + (blurDir * vec2(1.0 / 3.0 - 0.5))).xyz + 
		texture(sceneColors, vUV + (blurDir * vec2(2.0 / 3.0 - 0.5))).xyz
	);

	vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
		texture(sceneColors, vUV + (blurDir * vec2(0.0 / 3.0 - 0.5))).xyz + 
		texture(sceneColors, vUV + (blurDir * vec2(3.0 / 3.0 - 0.5))).xyz
	);

	// Find the min and max to see if we are going out of range.
	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));

	float lumaResult2 = dot(luma, result2);
	
	if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
		outColor = vec4(result1, 1.0);
	else
		outColor = vec4(result2, 1.0);

	gl_FragDepth = texture(sceneDepth, vUV).r;
	//outColor = texture(sceneColors, vUV);
}
