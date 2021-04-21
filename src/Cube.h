#pragma once
#include "Entity.h"

class Cube : public Entity {
public:
	Cube(const glm::vec3& minBound, const glm::vec3& maxBound);
	~Cube() override;
	void draw() const override;

private:
	void constructModel(const glm::vec3& minBound, const glm::vec3& maxBound);
	GLuint indicesCount = 0;
	GLuint ebo = 0;
};