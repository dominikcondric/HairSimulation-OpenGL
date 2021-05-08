#include "Sphere.h"
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>

Sphere::Sphere(unsigned int sectorCount, unsigned int stackCount, float _radius) : radius(_radius)
{
	vertexCount = (sectorCount + 1) * (stackCount + 1);
	indicesCount = ((stackCount - 2) * sectorCount * 6) + (2 * sectorCount * 3);
	constructModel(sectorCount, stackCount);
}

Sphere::~Sphere()
{
	glDeleteBuffers(1, &ebo);
}

void Sphere::draw() const
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Sphere::constructModel(uint32_t sectorCount, uint32_t stackCount)
{
	const float len = 1.f / radius;
	std::vector<glm::vec3> vertices;
	vertices.reserve(vertexCount);
	constexpr float pi = glm::pi<float>();
	float sectorStep = 2 * pi / sectorCount;
	float stackStep = pi / stackCount;
	float sectorAngle, stackAngle;
	float xy;
	glm::vec3 vertex;
	for (uint32_t i = 0; i <= stackCount; ++i)
	{
		stackAngle = pi / 2 - i * stackStep;
		xy = radius * cos(stackAngle);
		vertex.z = radius * sin(stackAngle);
		for (uint32_t j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;
			vertex.x = xy * cos(sectorAngle);
			vertex.y = xy * sin(sectorAngle);
			vertices.push_back(vertex);
		}
	}

	std::vector<GLuint> indices;
	std::vector<float> data;

	indices.reserve(indicesCount);
	data.reserve(8 * vertexCount);

	int k1, k2;
	for (uint32_t i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;
		for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	int k = 0;
	for (uint32_t i = 0; i <= stackCount; ++i)
	{
		for (uint32_t j = 0; j <= sectorCount; ++j)
		{
			data.push_back(vertices[k].x);
			data.push_back(vertices[k].y);
			data.push_back(vertices[k].z);

			data.push_back(vertices[k].x * len);
			data.push_back(vertices[k].y * len);
			data.push_back(vertices[k].z * len);

			data.push_back((float)j / sectorCount);
			data.push_back((float)i / stackCount);
			++k;
		}
	}

	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)sizeof(glm::vec3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
