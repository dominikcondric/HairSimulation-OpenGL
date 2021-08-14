#version 460 core

layout (location = 0) in vec3 inPosition;

out Attributes {
	vec3 fragPosition;
	vec3 tangent;
} outAttributes;

uniform mat4 model;
uniform uint particlesPerStrand;

void main() 
{
	if (gl_VertexID % particlesPerStrand == 0)
		outAttributes.fragPosition = vec3(model * vec4(inPosition, 1.f));
	else
		outAttributes.fragPosition = inPosition;

	gl_Position = vec4(outAttributes.fragPosition, 1.f);
}