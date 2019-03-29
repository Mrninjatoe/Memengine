#version 440 core
#extension GL_EXT_texture_array : enable

in vec3 vPos;
in vec2 vUV;

layout(location = 0) out vec4 outColor;

struct Pointlight{
	vec3 pos;
	vec3 color;
};

const int MAX_LIGHTS = 128;
const int MAX_NUM_CASCADES = 8;

layout(location = 15) uniform float variancePower;
layout(location = 16) uniform float lowVSMValue;
layout(location = 17) uniform float highVSMValue;
layout(location = 18) uniform vec3 shadowCasterPos;
layout(location = 19) uniform vec3 cameraPos;
layout(location = 20) uniform sampler2D positions;
layout(location = 21) uniform sampler2D normals;
layout(location = 22) uniform sampler2D colors;
layout(location = 23) uniform sampler2D depths;
layout(location = 24) uniform sampler2DArray shadowDepths;

layout(location = 25) uniform ivec2 screenSize;
layout(location = 26) uniform mat4 v;
layout(location = 27) uniform bool debugCascade;

layout(location = 28) uniform int numOfCascades;
layout(location = 29) uniform float cascadedSplits[MAX_NUM_CASCADES];
layout(location = 37) uniform mat4 lightViewProjs[MAX_NUM_CASCADES];

layout(location = 45) uniform int numberOfLights;
layout(location = 46) uniform Pointlight pointLights[MAX_LIGHTS];

float linstep(float low, float high, float v){
	return clamp((v-low)/(high-low), 0.0, 1.0);
}

float sampleVarianceShadowMap(vec3 projCoords, float compare){
	vec2 moments = texture2DArray(shadowDepths, projCoords).xy;
	float p = step(compare, moments.x);
	float variance = max(moments.y - moments.x * moments.x, variancePower);
	float dFromMean = compare - moments.x;
	float pMax = linstep(lowVSMValue, highVSMValue, variance / (variance + dFromMean*dFromMean));
	return min(max(p, pMax), 1.0f);
}

vec4 readShadowMap(vec3 lightDir, vec3 fragNormal, vec3 fragPos){
	vec3 fragVPos = vec3(v * vec4(fragPos, 1.0f));
	float posViewSpaceZ = fragVPos.z;

	uint cascadeIndex = 0;
	for(uint i = 0; i < numOfCascades - 1; i++){
		if(posViewSpaceZ < cascadedSplits[i])
			cascadeIndex = i + 1;
	}
	
	mat4 lightViewProjMX = lightViewProjs[cascadeIndex];
	vec4 fragShadowPos = lightViewProjMX * vec4(fragPos, 1.f);

	vec3 projCoords = fragShadowPos.xyz / fragShadowPos.w;
	projCoords = projCoords * 0.5f + 0.5f;
	float currentDepth = projCoords.z;
	projCoords.z = cascadeIndex;

	// PCF
	float shadow = 0.f;
	shadow = sampleVarianceShadowMap(projCoords, currentDepth);

	if(currentDepth > 1.0f)
		shadow = 1.f;

	return vec4(vec3(shadow), projCoords.z);	
}

vec4 calculateDirLight(){
	vec4 fragPos = texture(positions, vUV);
	vec3 fragNormal = texture(normals, vUV).xyz;
	vec4 fragColor = texture(colors, vUV);
	vec4 fragDepth = texture(depths, vUV);

	vec3 lightDir = normalize(shadowCasterPos);
	fragNormal = normalize(fragNormal);
	// Ambient
	vec3 ambient = (fragColor.rgb * 0.1f * pointLights[0].color);

	// Specular
	vec3 viewDir = normalize(cameraPos - fragPos.xyz);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(fragNormal, halfWayDir), 0.0), 16);
	vec3 specular = spec * pointLights[0].color * 0.2f;

	// Diffuse
	float diff = max(0.f, dot(fragNormal, lightDir));
	vec3 diffuse = diff * pointLights[0].color;
	
	vec4 shadow = readShadowMap(lightDir, fragNormal, fragPos.xyz);

	return vec4(ambient + (shadow.xyz * (diffuse + specular) * fragColor.rgb), shadow.w);
}

void main(){
	vec4 result = vec4(0);

	result = calculateDirLight();
	vec3 cascadeColor = vec3(0);
	if(debugCascade){
		if(result.w == 0)
			cascadeColor = vec3(0.2,0,0);
		else if(result.w == 1)
			cascadeColor = vec3(0,0.2,0);
		else if(result.w == 2)
			cascadeColor = vec3(0,0,0.2);
		else if(result.w == 3)
			cascadeColor = vec3(0.2, 0, 0.2);
		else if(result.w == 4)
			cascadeColor = vec3(0.2, 0.2, 0);
	}
//	float alphaTemp = texture(colors, vUV).a;
//	if(alphaTemp < 0.1)
//		discard;

	//outColor = vec4(cascadedSplits[0], cascadedSplits[1], cascadedSplits[2], 1);
	//outColor = texture(normals, vUV);
	outColor = vec4(result.rgb, 1);
	//gl_FragDepth = texture(depths, vUV).r; // Output depth into framebuffer 0.
}