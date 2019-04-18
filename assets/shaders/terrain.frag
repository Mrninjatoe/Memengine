#version 440 core

layout(location = 0) out vec4 outPos;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out float outDepth;

in vec3 vPos;
in vec3 vNormal;
in vec2 vUV;
in vec3 vColor;

layout(location = 21) uniform sampler2D lilDiffTex;

void main(){
	outPos = vec4(vPos, 1);
	outNormal = vec4(vNormal, 0);
	vec4 texColor = texture(lilDiffTex, vUV);
	outColor = vec4(vColor + texColor.rgb, 1);
	
	// outDepth = gl_FragCoord.z Don't need since depth/stencil defined.
}