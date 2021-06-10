#version 460 core
#define VERTICES_BETWEEN 10

layout (lines) in;
layout(line_strip, max_vertices = VERTICES_BETWEEN) out;

in Attributes {
	vec3 fragPosition;
	vec3 tangent;
} inAttributes[];

out Attributes {
	vec3 fragPosition;
	vec3 tangent;
} outAttributes;

uniform mat4 projection;
uniform mat4 view;
uniform float curlRadius = 0.05f;

void main(void)
{
	if (curlRadius != 0.f)
	{
		const float segmentLength = length(gl_in[0].gl_Position - gl_in[1].gl_Position) / (VERTICES_BETWEEN - 1);
		const vec3 startingPosition = gl_in[0].gl_Position.xyz;
		const vec3 endingPosition = gl_in[1].gl_Position.xyz;
		const vec3 hairDirection = normalize(endingPosition - startingPosition);
		const vec3 uDirection = normalize(cross(vec3(hairDirection), vec3(0.f, 1.f, 0.f)));
		const vec3 vDirection = normalize(cross(vec3(hairDirection), uDirection));
		float angleStep = 360.f / (VERTICES_BETWEEN-1);

		uint i = 0;
		vec3 nextPosition;
		nextPosition = startingPosition + hairDirection * segmentLength * i;
		nextPosition += curlRadius * uDirection * cos(radians(angleStep) * i);
		nextPosition += curlRadius * vDirection * sin(radians(angleStep) * i);

		for (i = 1; i <= VERTICES_BETWEEN; ++i) 
		{
			outAttributes.fragPosition = nextPosition;
			gl_Position = vec4(nextPosition, 1.f);
			gl_Position = projection * view * gl_Position;
			
			nextPosition = startingPosition + hairDirection * segmentLength * i;
			nextPosition += curlRadius * uDirection * cos(radians(angleStep) * i);
			nextPosition += curlRadius * vDirection * sin(radians(angleStep) * i);
			
			outAttributes.tangent = normalize(nextPosition - outAttributes.fragPosition);

			EmitVertex();
		}

		gl_Position = vec4(nextPosition, 1.f);
		gl_Position = projection * view * gl_Position;
		outAttributes.fragPosition = nextPosition;
		EmitVertex();
		EndPrimitive();
	} 
	else 
	{
		gl_Position = projection * view * vec4(gl_in[0].gl_Position.xyz, 1.f);
		outAttributes.fragPosition = inAttributes[0].fragPosition;
		outAttributes.tangent = normalize(inAttributes[1].fragPosition - inAttributes[0].fragPosition);
		EmitVertex();

		gl_Position = projection * view * vec4(gl_in[1].gl_Position.xyz, 1.f);
		outAttributes.fragPosition = inAttributes[1].fragPosition;
		outAttributes.tangent = normalize(inAttributes[1].fragPosition - inAttributes[0].fragPosition);
		EmitVertex();

		EndPrimitive();
	}
}