#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

in Attributes {
	vec3 fragPosition;
	vec3 tangent;
} inAttributes;

uniform Light light;
uniform vec3 eyePosition;
uniform Material material;

float attenuation(in Light light) 
{
	float distance = length(light.position - inAttributes.fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance * light.quadratic * distance * distance);
	return attenuation;
}

vec4 calculatePointLight() 
{
	// ambient
	vec3 ambientComponent = material.ambient; 

	// diffuse
	vec3 lightDirection = normalize(light.position - inAttributes.fragPosition);
	float lightAngle = acos(abs(dot(lightDirection, inAttributes.tangent)));
	vec3 diffuseComponent = material.diffuse * light.color * sin(lightAngle);

	// specular
	vec3 eyeDirection = normalize(eyePosition - inAttributes.fragPosition);
	float eyeAngle = acos(abs(dot(eyeDirection, inAttributes.tangent)));
	vec3 specularComponent = material.specular * light.color * pow(cos(lightAngle - eyeAngle), material.shininess);

	vec4 result = vec4((ambientComponent + diffuseComponent + specularComponent) * attenuation(light), 1.f);

	return result;
}

void main() 
{
	gl_FragColor = calculatePointLight();
}