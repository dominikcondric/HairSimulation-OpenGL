#pragma once
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include "Shader.h"

class Entity {
public:
	Entity();
	virtual ~Entity();
	virtual void draw() const = 0;
	void rotate(float angle, const glm::vec3& axis);
	void scale(const glm::vec3& factor);
	void translate(const glm::vec3& factor);
	const glm::mat4& getTransformMatrix() const { return transformMatrix; }
	const glm::vec3& getScale() const { return scaleVector; }
	const glm::vec3& getTranslation() const { return translationVector; }
	const glm::quat& getRotationQuat() const { return rotationQuat; }
	glm::vec3 color{ 1.f };

	enum class Material {
		PLASTIC,
		METAL,
		FABRIC,
		HAIR
	};

	void updateColorsBasedOnMaterial(const Shader& shader, Material material) const;

protected:
	glm::mat4 transformMatrix{ 1.f };
	glm::quat rotationQuat;
	glm::vec3 translationVector{ 0.f };
	glm::vec3 scaleVector{ 1.f };
	GLuint vbo = 0;
	GLuint vao = 0;
	GLuint vertexCount = 0;
};

