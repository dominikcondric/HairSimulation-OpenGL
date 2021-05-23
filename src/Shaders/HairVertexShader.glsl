#version 460 core

layout (location = 0) in vec3 inPosition;

out Attributes {
	vec3 fragPosition;
	vec3 tangent;
} outAttributes;

void main() 
{
	outAttributes.fragPosition = inPosition;
	gl_Position = vec4(inPosition, 1.0);
}