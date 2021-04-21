#include "Cube.h"
#include <vector>

Cube::Cube(const glm::vec3& minBound, const glm::vec3& maxBound)
{
	indicesCount = 36;
	vertexCount = 24;
	constructModel(minBound, maxBound);
}

Cube::~Cube()
{
	glDeleteBuffers(1, &ebo);
}

void Cube::draw() const
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Cube::constructModel(const glm::vec3& minBound, const glm::vec3& maxBound)
{
	std::vector<glm::vec3> vertices = {
		maxBound,
		glm::vec3(minBound.x, maxBound.y, maxBound.z),
		glm::vec3(minBound.x,minBound.y, maxBound.z),
		glm::vec3(maxBound.x,minBound.y, maxBound.z),
		// left side
		glm::vec3(minBound.x, maxBound.y, maxBound.z),
		glm::vec3(minBound.x, maxBound.y,minBound.z),
		minBound,
		glm::vec3(minBound.x,minBound.y, maxBound.z),
		// back side
		glm::vec3(maxBound.x, maxBound.y,minBound.z),
		glm::vec3(minBound.x, maxBound.y,minBound.z),
		minBound,
		glm::vec3(maxBound.x,minBound.y,minBound.z),
		// right side
		maxBound,
		glm::vec3(maxBound.x, maxBound.y,minBound.z),
		glm::vec3(maxBound.x,minBound.y,minBound.z),
		glm::vec3(maxBound.x,minBound.y, maxBound.z),
		// top side
		glm::vec3(maxBound.x, maxBound.y,minBound.z),
		glm::vec3(minBound.x, maxBound.y,minBound.z),
		glm::vec3(minBound.x, maxBound.y, maxBound.z),
		maxBound,
		// bottom side
		glm::vec3(maxBound.x,minBound.y,minBound.z),
		minBound,
		glm::vec3(minBound.x,minBound.y, maxBound.z),
		glm::vec3(maxBound.x,minBound.y, maxBound.z)
	};

	const GLuint indices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 11, 10, 8, 10, 9, 12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21 };
	const glm::vec3 normals[] = {
		// front side
		glm::vec3(0.f, 0.f, 1.f),
		glm::vec3(0.f, 0.f, 1.f),
		glm::vec3(0.f, 0.f, 1.f),
		glm::vec3(0.f, 0.f, 1.f),
		// left side
		glm::vec3(-1.f, 0.f, 0.f),
		glm::vec3(-1.f, 0.f, 0.f),
		glm::vec3(-1.f, 0.f, 0.f),
		glm::vec3(-1.f, 0.f, 0.f),
		// back side
		glm::vec3(0.f, 0.f, -1.f),
		glm::vec3(0.f, 0.f, -1.f),
		glm::vec3(0.f, 0.f, -1.f),
		glm::vec3(0.f, 0.f, -1.f),
		// right side
		glm::vec3(1.f, 0.f, 0.f),
		glm::vec3(1.f, 0.f, 0.f),
		glm::vec3(1.f, 0.f, 0.f),
		glm::vec3(1.f, 0.f, 0.f),
		// top side
		glm::vec3(0.f, 1.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f),
		// bottom side
		glm::vec3(0.f, -1.f, 0.f),
		glm::vec3(0.f, -1.f, 0.f),
		glm::vec3(0.f, -1.f, 0.f),
		glm::vec3(0.f, -1.f, 0.f),
	};

	const glm::vec2 texCoords[] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
	};

	std::vector<float> data;
	data.reserve(vertexCount * 8);
	for (size_t i = 0; i < 24; ++i)
	{
		data.push_back(vertices[i].x);
		data.push_back(vertices[i].y);
		data.push_back(vertices[i].z);
		data.push_back(normals[i].x);
		data.push_back(normals[i].y);
		data.push_back(normals[i].z);
		data.push_back(texCoords[i].x);
		data.push_back(texCoords[i].y);
	}

	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)sizeof(glm::vec3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
