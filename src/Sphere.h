#pragma once
#include "Entity.h"

class Sphere : public Entity {
public:
	Sphere(unsigned int sectorCount, unsigned int stackCount, float radius);
	~Sphere() override;
	void draw() const override;
	float getRadius() const { return radius * scaleVector.x; };

private:
	void constructModel(size_t sectorCount, size_t stackCount);
	GLuint ebo = 0.f;
	float radius = 1.f;
	GLuint indicesCount = 0;
};

