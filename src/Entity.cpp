#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

Entity::Entity() : rotationQuat(glm::angleAxis(0.f, glm::vec3(1.f, 0.f, 0.f)))
{
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
}

Entity::~Entity()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Entity::rotate(float angle, const glm::vec3& axis)
{
	rotationQuat = glm::angleAxis(glm::radians(angle), axis);
	transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * glm::mat4_cast(rotationQuat), scaleVector);
}

void Entity::scale(const glm::vec3& scale)
{
	scaleVector = scale;
	transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * glm::mat4_cast(rotationQuat), scaleVector);
}

void Entity::translate(const glm::vec3& translation)
{
	translationVector = translation;
	transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * glm::mat4_cast(rotationQuat), scaleVector);
}

void Entity::updateColorsBasedOnMaterial(const Shader& shader, Material material) const
{
	switch (material) 
	{
		case Material::PLASTIC:
			shader.setVec3("material.ambient", 0.2f * color);
			shader.setVec3("material.diffuse", 0.8f * color);
			shader.setVec3("material.specular", color);
			shader.setFloat("material.shininess", 10.f);
			break;
		case Material::METAL:
			shader.setVec3("material.ambient", 0.8f * color);
			shader.setVec3("material.diffuse", color);
			shader.setVec3("material.specular", color);
			shader.setFloat("material.shininess", 200.f);
			break;
		case Material::FABRIC:
			shader.setVec3("material.ambient", 0.2f * color);
			shader.setVec3("material.diffuse", color);
			shader.setVec3("material.specular", color * 0.05f);
			shader.setFloat("material.shininess", 1.f);
			break;
	}
}