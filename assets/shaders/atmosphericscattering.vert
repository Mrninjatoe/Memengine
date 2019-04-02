// Based of GPGPU gem white paper:
// https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter16.html
// Code adapted from: 
// https://github.com/Scrawk/GPU-GEMS-Atmospheric-Scatter/blob/master/Assets/AtmosphericScattingONeils/Shaders/SkyFromAtmosphere.shader

#version 440 core
layout(location = 0) in vec3 pos;
layout(location = 4) in vec2 uv;

out vec3 vPos;
out vec2 vUV;

layout(location = 0) uniform mat4 v;
layout(location = 1) uniform mat4 p;
layout(location = 2) uniform mat4 m;
layout(location = 3) uniform vec3 cameraPos;

void main(){
	vPos = vec3(m * vec4(pos, 1));
	vUV = uv;
	vec4 convertPos = p * v * vec4(pos + cameraPos, 1);
	gl_Position = convertPos.xyww;
}