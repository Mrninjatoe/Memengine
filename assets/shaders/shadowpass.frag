#version 440 core
// https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5

layout(location = 0) out vec2 fragColor;

in vec2 gsUV;

layout(location = 20) uniform sampler2D diffTex;


void main(){
	if(texture(diffTex, gsUV).a < 0.1f)
		discard;

	float depth = gl_FragCoord.z;
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float depthSquared = depth * depth + 0.25 * (dx * dx + dy * dy);
	
	fragColor = vec2(depth, depthSquared);
	// gl_FragDepth = gl_FragCoord.z
}