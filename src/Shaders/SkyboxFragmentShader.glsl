#version 330 core

in vec3 direction;

out vec4 color;

uniform samplerCube cubeSampler;
uniform int blurFactor = 200;

void main()
{
	float offset = 1.0 / (1000 - blurFactor);
	vec3 offsets[9] = vec3[](
		vec3(-offset, offset, -offset),
		vec3(0.0, offset, 0.0),
		vec3(offset, offset, offset),
		vec3(-offset, 0.0, -offset),
		vec3(0.0, 0.0, 0.0),
		vec3(offset, 0.0, offset),
		vec3(-offset, -offset, -offset),
		vec3(0.0, -offset, 0.0),
		vec3(offset, -offset, offset)
	);

	float blurKernel[9] = float[](
		1 / 16.0, 2 / 16.0, 1 / 16.0,
		2 / 16.0, 4 / 16.0, 2 / 16.0,
		1 / 16.0, 2 / 16.0, 1 / 16.0
	);

	vec3 col = vec3(0.0);
	for (int i = 0; i < 9; ++i)
		col += texture(cubeSampler, direction + offsets[i]).rgb * blurKernel[i];

	color = vec4(col, 1.0);
}
