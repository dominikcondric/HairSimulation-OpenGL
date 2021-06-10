#pragma once
#include "Entity.h"

class Sphere : public Entity {
public:
	Sphere(unsigned int sectorCount, unsigned int stackCount, float radius);
	~Sphere() override;
	void draw() const override;
	float getRadius() const { return radius; };

private:
	void constructModel(uint32_t sectorCount, uint32_t stackCount);
	GLuint ebo = 0U;
	float radius = 1.f;
	GLuint indicesCount = 0;
};

