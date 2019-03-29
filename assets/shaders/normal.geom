#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData{

} inVertices[];


void main(){
	vPos = vec3(m * vec4(pos, 1)).xyz;
	vNormal = vec3(m * vec4(normal, 1)).xyz;
	vColor = color;
	vUV = uv;
	gl_Position =  p * v * m * vec4(vPos, 1.0f);
}