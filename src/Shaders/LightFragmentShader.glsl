#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D diffuseMap;
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
	vec3 normal;
	vec2 texCoords;
} inAttributes;

uniform bool tex = false;
uniform Light light;
uniform vec3 eyePosition;
uniform Material material;

float attenuation(in Light light) 
{
	float distance = length(light.position - inAttributes.fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance * light.quadratic * distance * distance);
	return attenuation;
}

vec4 calculatePointLight(in vec4 colorDiffuse, in vec4 colorSpecular, in vec4 colorAmbient, in vec3 eyeDirection, in Light light) 
{
	// ambient
	vec3 ambientComponent = vec3(colorAmbient) * 0.3; 

	// diffuse
	vec3 lightDirection = normalize(light.position - inAttributes.fragPosition);
	float lightAngle = dot(lightDirection, inAttributes.normal);
	lightAngle = max(lightAngle, 0.0);
	vec3 diffuseComponent = vec3(colorDiffuse) * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection, inAttributes.normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, inAttributes.normal), 0.0), 3 * material.shininess);
	vec3 specularComponent = vec3(colorSpecular) * light.color * eyeAngle;
		
	vec4 result = vec4((ambientComponent + diffuseComponent + specularComponent) * attenuation(light), (colorDiffuse.a + colorSpecular.a) / 2.0);

	return result;
}

void main() 
{
	vec4 colorDiffuse;
	vec4 colorSpecular;
	vec4 colorAmbient;
	if (tex) 
	{
		colorDiffuse = texture(material.diffuseMap, inAttributes.texCoords);
		colorSpecular = colorDiffuse * 0.7;
		colorAmbient = vec4(colorDiffuse.rgb * 0.1, colorDiffuse.a);
		if (colorDiffuse.a < 0.1 || colorSpecular.a < 0.1) discard;
	}
	else 
	{
		colorAmbient = vec4(material.ambient, 1.0);
		colorDiffuse = vec4(material.diffuse, 1.0);
		colorSpecular = vec4(material.specular, 1.0);
	}

	vec3 eyeDirection = normalize(eyePosition - inAttributes.fragPosition);
	gl_FragColor = calculatePointLight(colorDiffuse, colorSpecular, colorAmbient, eyeDirection, light);
}