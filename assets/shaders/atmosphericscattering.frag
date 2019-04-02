#version 440 core
in vec3 vPos;
in vec2 vUV;

out vec4 outColor;

layout(location = 3) uniform vec3 cameraPos;
layout(location = 4) uniform vec3 lightDir;
layout(location = 5) uniform vec3 wlRGB;
layout(location = 6) uniform float cameraHeight;
layout(location = 7) uniform float cameraHeight2;
layout(location = 8) uniform float outerRadius;
layout(location = 9) uniform float outerRadius2;
layout(location = 10) uniform float innerRadius;
layout(location = 11) uniform float innerRadius2;
layout(location = 12) uniform float fKrESun;
layout(location = 13) uniform float fKmESun;
layout(location = 14) uniform float fKr4PI;
layout(location = 15) uniform float fKm4PI;
layout(location = 16) uniform float fScale;
layout(location = 17) uniform float scaleDepth; // must be 0.25f (25% of the density)
layout(location = 18) uniform float scaleOverScaleDepth; //fscale/scaledepth
layout(location = 19) uniform vec3 atmospherePos;
layout(location = 20) uniform float g;
layout(location = 21) uniform float g2;
layout(location = 25) uniform sampler2D imageColors;
layout(location = 26) uniform sampler2D imagePositions;

float getNearIntersection(vec3 ray){
	float B = 2.0 * dot(cameraPos, ray);
	float C = cameraHeight2 - outerRadius2;
	float det = max(0.0, B*B - 4.0 * C);

	return 0.5f * (-B - sqrt(det));
}

float scale(float cosAngle){
	float x = 1.0 - cosAngle;
	return 0.25 * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

float getMiePhase(float cosAngle, float cosAngle2, float g, float g2){
	return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + cosAngle2) / pow(1.0 + g2 - 2.0 * g * cosAngle, 1.5);
}

float getRayleighPhase(float cosAngle2){
	return 0.75 + 0.75 * cosAngle2;
}

void main(){
	vec3 sceneColor = texture(imageColors, vUV).rgb;
	vec3 atmosphereOffset = vec3(0, innerRadius, 0);
	
	vec3 fragPos = vPos + atmosphereOffset;
	vec3 offsetCameraPos = atmosphereOffset;
	vec3 vertToCameraRay = fragPos - offsetCameraPos;
	float offsetCameraHeight = clamp(length(offsetCameraPos.y),0, outerRadius);
	
	float far = length(vertToCameraRay);
	vertToCameraRay /= far;

	vec3 rayStart = offsetCameraPos;
	float height = length(rayStart);
	float depth = exp(scaleOverScaleDepth * (innerRadius - offsetCameraHeight));
	float startAngle = dot(vertToCameraRay, rayStart) / height;
	float startOffset = depth * scale(startAngle);

	const float numSamples = 100.f;

	float sampleLength = far / numSamples;
	float scaledLength = sampleLength * fScale;
	vec3 sampleRay = vertToCameraRay * sampleLength;
	vec3 samplePoint = rayStart + sampleRay * 0.5;

	vec3 frontColor = vec3(0.f);
	for(int i = 0; i < int(numSamples); i++){
		float cSampleHeight = length(samplePoint);
		float cSampleDepth = exp(scaleOverScaleDepth * (innerRadius - cSampleHeight));
		float cLightAngle = dot(-lightDir, samplePoint) / cSampleHeight;
		float cCameraAngle = dot(vertToCameraRay, samplePoint) / cSampleHeight;
		float cScatter = startOffset + cSampleDepth * (scale(cLightAngle) - scale(cCameraAngle));
		vec3 cAttenuation = exp(-cScatter * (wlRGB * fKr4PI + fKm4PI));
		frontColor += cAttenuation * (cSampleDepth * scaledLength);
		samplePoint += sampleRay;
	}

	vec3 c0 = frontColor * (wlRGB * fKrESun); // Rayleigh
	vec3 c1 = frontColor * fKmESun; // Mie

	vec3 t0 = offsetCameraPos - fragPos;

	float cosAngle = dot(-lightDir, t0) / length(t0);
	float cosAngle2 = cosAngle * cosAngle;
	vec3 col = (getRayleighPhase(cosAngle2) * c0) + getMiePhase(cosAngle, cosAngle2, g, g2) * c1;
	//col *= vec3(0.29296875, 0, 0.5078125);
	col = 1.0 - exp(col * -0.8f);

	outColor = vec4(col, 1);
}