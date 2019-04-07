#version 440 core
#extension GL_EXT_texture_array : enable

in vec3 vPos;
in vec2 vUV;

layout(location = 0) out vec4 outColor;

struct Pointlight{
	vec3 pos;
	vec3 color;
};

const int MAX_LIGHTS = 16;
const int MAX_NUM_CASCADES = 8;

layout(location = 15) uniform float variancePower;
layout(location = 16) uniform float lowVSMValue;
layout(location = 17) uniform float highVSMValue;
layout(location = 18) uniform vec3 lightDir;
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

layout(location = 47) uniform vec3 wlRGB;
layout(location = 48) uniform float cameraHeight;
layout(location = 49) uniform float cameraHeight2;
layout(location = 50) uniform float outerRadius;
layout(location = 51) uniform float outerRadius2;
layout(location = 52) uniform float innerRadius;
layout(location = 53) uniform float innerRadius2;
layout(location = 54) uniform float fKrESun;
layout(location = 55) uniform float fKmESun;
layout(location = 56) uniform float fKr4PI;
layout(location = 57) uniform float fKm4PI;
layout(location = 58) uniform float fScale;
layout(location = 59) uniform float scaleDepth; // must be 0.25f (25% of the density)
layout(location = 60) uniform float scaleOverScaleDepth; //fscale/scaledepth
layout(location = 61) uniform vec3 atmospherePos;
layout(location = 62) uniform float g;
layout(location = 63) uniform float g2;
layout(location = 64) uniform float scatteringNumSamples;

layout(location = 80) uniform int numberOfLights;
layout(location = 81) uniform Pointlight pointLights[MAX_LIGHTS];

layout(location = 400) uniform float hdrExposure;

float scale(float cosAngle){
	float x = 1.0 - cosAngle;
	return 0.25 * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

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

vec4 readShadowMap(vec3 fragNormal, vec3 fragPos){
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

vec4 calculateDirLight(vec4 fragColor, vec4 fragPos, vec3 fragNormal){
	vec3 sunColor = vec3(1);

	// Specular
	vec3 viewDir = normalize(cameraPos - fragPos.xyz);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(fragNormal, halfWayDir), 0.0), 16);
	vec3 specular = spec * sunColor * 0.2f;

	// Diffuse
	float diff = max(0.f, dot(fragNormal, lightDir));
	vec3 diffuse = diff * sunColor;
	
	vec4 shadow = readShadowMap(fragNormal, fragPos.xyz);

	return vec4((shadow.xyz * (diffuse) * fragColor.rgb), shadow.w);
}

vec4 calculateScattering(vec3 currentColor, vec3 pos){
	vec3 col;
	vec3 atmosphereOffset = vec3(0, innerRadius, 0);
	
	// toOffset displaces the cameraPosition lower/higher depending on the cos angle
	// between the "sun position"  and a global up vector.
	float toOffset = clamp(dot(lightDir, vec3(0, 1, 0)) * 25.f, -25.f, -5.2f);
	vec3 offsetCameraPos = cameraPos + atmosphereOffset;
	float offsetCameraHeight = clamp(length(offsetCameraPos.y), innerRadius + 25, outerRadius);
	
	vec3 fragPos = pos + atmosphereOffset;
	vec3 vertToCameraRay = fragPos - offsetCameraPos;
	fragPos = normalize(fragPos);
	float far = length(vertToCameraRay);
	vertToCameraRay /= far;

	vec3 rayStart = offsetCameraPos;
	float depth = exp((innerRadius - offsetCameraHeight) * (1.0/scaleDepth));
	float cameraAngle = dot(-vertToCameraRay, fragPos);
	float lightAngle = dot(lightDir, fragPos);
	float cameraScale = scale(cameraAngle);
	float lightScale = scale(lightAngle);
	float cameraOffset = depth * cameraScale;
	float temp = (lightScale + cameraScale);

	float sampleLength = far / scatteringNumSamples;
	float scaledLength = sampleLength * fScale;
	vec3 sampleRay = vertToCameraRay * sampleLength;
	vec3 samplePoint = rayStart + sampleRay * 0.5f;

	vec3 frontColor = vec3(0.f);
	vec3 attenuation;
	for(int i = 0; i < int(scatteringNumSamples); i++){
		float cHeight = length(samplePoint);
		float cDepth = exp(scaleOverScaleDepth * (innerRadius - cHeight));
		float cScatter = cDepth * temp - cameraOffset;
		attenuation = exp(-cScatter * (wlRGB * fKr4PI + fKm4PI));
		frontColor += attenuation * (cDepth * scaledLength);
		samplePoint += sampleRay;
	}
	
	vec3 c0 = frontColor * (wlRGB * fKrESun + fKmESun);
	vec3 c1 = attenuation;

	col = c0 + currentColor * c1;

	return vec4(col, 1);
}

vec3 calculatePointLight(vec3 fragColor, vec3 fragPos, vec3 fragNormal, Pointlight pointLight){
	vec3 col = vec3(0);

	vec3 pLightDir = normalize(pointLight.pos - fragPos);

	// Specular
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 halfWayDir = normalize(pLightDir + viewDir);
	float spec = pow(max(dot(fragNormal, halfWayDir), 0.0), 128);
	vec3 specular = spec * pointLight.color * 0.2f;

	// Diffuse
	float diff = max(dot(fragNormal, pLightDir), 0.0);
	vec3 diffuse = diff * pointLight.color;

	col = (specular + diffuse) * fragColor;

	return col;
}

void main(){
	vec4 result = vec4(0);
	vec4 fragColor = texture(colors, vUV);
	vec4 fragPos = texture(positions, vUV);
	vec3 fragNormal = normalize(texture(normals, vUV).xyz);

	result = calculateDirLight(fragColor, fragPos, fragNormal);
	int cascadeDebug = int(result.w);
	result = calculateScattering(result.rgb, fragPos.xyz);

	float lightFallOff = 1.f / numberOfLights;
	for(int i = 0; i < numberOfLights; i++){
		result.rgb += lightFallOff * calculatePointLight(fragColor.rgb, fragPos.xyz, fragNormal, pointLights[i]);
	}

	vec3 ambient = (fragColor.rgb * 0.1f);
	result.rgb += ambient;

	vec3 cascadeColor = vec3(0);
	if(debugCascade){
		if(cascadeDebug == 0)
			cascadeColor = vec3(0.2,0,0);
		else if(cascadeDebug == 1)
			cascadeColor = vec3(0,0.2,0);
		else if(cascadeDebug == 2)
			cascadeColor = vec3(0,0,0.2);
		else if(cascadeDebug == 3)
			cascadeColor = vec3(0.2, 0, 0.2);
		else if(cascadeDebug == 4)
			cascadeColor = vec3(0.2, 0.2, 0);
	}

	result = 1.0 - exp(result * hdrExposure);

	outColor = vec4(result.rgb + cascadeColor, 1);
}