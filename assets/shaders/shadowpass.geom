#version 440 core

// Invocations is the only heckin thing u need to change for a change of number of cascades
// When I stop being lazy I might write to shader whenever it changes /shrug.

//layout(triangles, invocations = 4) in;
//layout(triangle_strip, max_vertices = 3) out;

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

const int MAX_NUM_CASCADES = 8;

in vec2 vsUV[];

out vec2 gsUV;

layout(location = 5) uniform mat4 lightSMatrix[MAX_NUM_CASCADES];

void main(){
	for(int layerIndex = 0; layerIndex < 4; layerIndex++){
		for(int i = 0; i < gl_in.length(); i++){
			gl_Layer = layerIndex;
			gl_Position = lightSMatrix[layerIndex] * gl_in[i].gl_Position;
			gsUV = vsUV[i];
			EmitVertex();
		}
		EndPrimitive();
	}
}