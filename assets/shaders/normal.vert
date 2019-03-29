#version 440 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec2 uv;
layout(location = 5) in mat4 modelMatrix;

out VS_OUT{
	vec3 pos;
	vec3 color;
	vec2 uv;
	mat3 TBN;
	vec3 tanFragPos;
	vec3 tanViewPos;
} vs_out;

layout(location = 0) uniform mat4 v;
layout(location = 1) uniform mat4 p;
layout(location = 2) uniform mat4 m;
layout(location = 3) uniform vec3 cameraPos;
layout(location = 4) uniform unsigned int instanced;

mat3 calcTBN(vec3 tangent, vec3 normal, mat4 matrix){
	mat3 normalMatrix = mat3(transpose(inverse(matrix))); // Leave out translation and scaling.
	vec3 T = normalize(normalMatrix * tangent);
	vec3 N = normalize(normalMatrix * normal);

	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N, T));

	return mat3(T, B, N); // Transforms from tangent to world.
}

void main(){
	mat4 tempM = modelMatrix;
	if(instanced == 1)
		tempM = m;

	vs_out.pos = vec3(tempM * vec4(pos, 1)).xyz;
	vs_out.color = color;
	vs_out.uv = uv;
	vs_out.TBN = calcTBN(tangent, normal, tempM);
	vs_out.tanFragPos = transpose(vs_out.TBN) * vs_out.pos; // transform from world to tangentspace.
	vs_out.tanViewPos = transpose(vs_out.TBN) * cameraPos; // -||-
	
	gl_Position = p * v * tempM * vec4(pos, 1);
}