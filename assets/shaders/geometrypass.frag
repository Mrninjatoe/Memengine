#version 440 core

in VS_OUT{
	vec3 pos;
	vec3 color;
	vec2 uv;
	mat3 TBN;
	vec3 tanFragPos;
	vec3 tanViewPos;
} fs_in;

layout(location = 0) out vec4 outPos;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out float outDepth;

layout(location = 10) uniform float heightScale;
layout(location = 11) uniform float minLayers;
layout(location = 12) uniform float maxLayers;
layout(location = 13) uniform bool hasParallax;
layout(location = 20) uniform sampler2D diffuseTexture;
layout(location = 21) uniform sampler2D normalTexture;
layout(location = 22) uniform sampler2D heightTexture;

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir){
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0,0,1.f), viewDir)));
	float layerDepth = 1.f / numLayers;
	float currLayerDepth = 0.f;
	vec2 P = viewDir.xy * heightScale;
	vec2 dTexCoords = P / numLayers;

	vec2 currTexCoords = texCoords;
	float currDepthValue = texture(heightTexture, currTexCoords).r;
	while(currLayerDepth < currDepthValue){
		currTexCoords -= dTexCoords;
		currDepthValue = texture(heightTexture, currTexCoords).r;
		currLayerDepth += layerDepth;
	}
	vec2 prevTexCoords = currTexCoords + dTexCoords;
	float afterDepth = currDepthValue - currLayerDepth;
	float beforeDepth = texture(heightTexture, prevTexCoords).r - currLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currTexCoords * (1.0 - weight);

	return currTexCoords;
}

void main(){
	vec2 texCoords = vec2(fs_in.uv);
	if(hasParallax)
		texCoords = parallaxMapping(vec2(fs_in.uv.x, 1.f - fs_in.uv.y), normalize(fs_in.tanViewPos - fs_in.tanFragPos));
	vec4 objectColor = texture(diffuseTexture, texCoords);

	if(objectColor.a < 0.5f)
		discard;

	vec3 normal = normalize(texture(normalTexture, texCoords).xyz * 2.0 - 1.0);
	outNormal = vec4(fs_in.TBN * normal, 0); // Transform from tangent to world space for lighting.

	outColor = vec4(objectColor.rgb, 1);
	outPos = vec4(fs_in.pos, 1); 
	// gl_FragDepth = gl_FragCoord.z; Don't need, happens because depth/stencil defined.
}