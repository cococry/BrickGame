#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexcoord;
layout (location = 2) in float aLighting;

out vec2 vTexcoord;

uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uModel;

out vec4 vVertex;
out float visibility;
out float vLighting;

const float density = 0.07f;
const float gradient = 12.0f;

void main() {
	vTexcoord = aTexcoord;
	vLighting = aLighting;
	vVertex = vec4(aPosition, 1.0);
	vec4 worldPosition = uModel * vec4(aPosition, 1.0);
	vec4 positionRelativeToCam = uView * worldPosition;

	gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);

	
	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}