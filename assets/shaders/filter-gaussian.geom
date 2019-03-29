#version 440 core

// AAAAAAAAAAAAAAAA
layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 uv;
} gs_in[];

out GS_OUT{
	vec2 uv;
	flat int layerIndex;
} gs_out;

void main(){
	for(int i = 0; i < 3; i++){
		gl_Layer = gl_InvocationID;
		gl_Position = gl_in[i].gl_Position;
		gs_out.uv = gs_in[i].uv;
		gs_out.layerIndex = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}