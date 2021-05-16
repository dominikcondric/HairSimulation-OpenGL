#version 460 core
#define VERTICES_BETWEEN 21

layout (lines) in;
layout(line_strip, max_vertices = VERTICES_BETWEEN) out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float curlRadius = 0.05f;

void main(void)
{
	const float segmentLength = length(gl_in[0].gl_Position - gl_in[1].gl_Position) / (VERTICES_BETWEEN - 1);
	const vec3 startingPosition = gl_in[0].gl_Position.xyz;
	const vec3 endingPosition = gl_in[1].gl_Position.xyz;
	const vec3 hairDirection = normalize(endingPosition - startingPosition);
	const vec3 uDirection = normalize(cross(vec3(hairDirection), vec3(0.f, 1.f, 0.f)));
	const vec3 vDirection = normalize(cross(uDirection, vec3(hairDirection)));
	float angleStep = 360.f / (VERTICES_BETWEEN - 1);
	for (uint i = 0; i < VERTICES_BETWEEN; ++i) 
	{
		gl_Position.xyz = startingPosition + hairDirection * segmentLength * i;
		gl_Position.xyz += curlRadius * uDirection * cos(radians(angleStep) * i);
		gl_Position.xyz += curlRadius * vDirection * sin(radians(angleStep) * i);
		gl_Position = projection * view * model * vec4(gl_Position.xyz, 1.f);
		EmitVertex();
	}
	EndPrimitive();
}