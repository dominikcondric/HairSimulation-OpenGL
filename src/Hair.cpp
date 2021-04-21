#include "Hair.h"
#include <vector>
#include <iostream>
#include <glm/gtc/random.hpp>

Hair::Hair(uint32_t _strandCount, uint32_t strandSegmentsCount, HairType type) : strandCount(_strandCount), computeShader("HairComputeShader.glsl")
{
	strandSegmentsCount = glm::clamp<uint32_t>(strandSegmentsCount, 2U, 49U);
	glGenBuffers(1, &velocityArrayBuffer);
	particlesPerStrand = strandSegmentsCount + 1;
	constructModel(type);
	GLuint localWorkGroupCountX = computeShader.getLocalWorkGroupsCount().x;
	globalWorkGroupCount = strandCount / localWorkGroupCountX;
	if (strandCount % localWorkGroupCountX != 0)
		globalWorkGroupCount += 1;

	computeShader.setGlobalWorkGroupCount(globalWorkGroupCount);
	computeShader.use();
	computeShader.setUint("hairData.strandCount", strandCount);
	computeShader.setUint("hairData.particlesPerStrand", particlesPerStrand);
	computeShader.setFloat("hairData.particleMass", 0.1f);
	computeShader.setFloat("hairData.segmentLength", 1.f / strandSegmentsCount);
	computeShader.setFloat("force.gravity", gravity);
	computeShader.setVec4("force.wind", wind);
}

Hair::~Hair()
{
	glDeleteBuffers(1, &velocityArrayBuffer);
}

void Hair::constructModel(HairType type)
{
	const float segmentLength = 1.f / (particlesPerStrand - 1);	
	std::vector<float> data;
	data.reserve(maximumStrandCount * 3);
	const float hairOffset = 0.005f;
	const float hairDistanceQuarter = (uint32_t)glm::sqrt(strandCount) * (hairOffset + 0.1f) / 4;
	/*const uint32_t xBlockSize = glm::floor(glm::sqrt(strandCount));
	const uint32_t yBlockSize = uint32_t(strandCount / xBlockSize) + (strandCount % xBlockSize);*/

	switch (type)
	{
		case Hair::HairType::Straight:
			for (uint32_t i = 0; i < maximumStrandCount; ++i) 
			{
				const glm::vec2 randomDiskCoordinates = glm::diskRand(1.f);
				for (uint32_t j = 0; j < particlesPerStrand; ++j)
				{
					data.push_back(randomDiskCoordinates.x);
					data.push_back(0.5f - segmentLength * j);
					data.push_back(randomDiskCoordinates.y);
				}
			}
			break;

		case Hair::HairType::Curly:
			std::cout << "Not yet implemented!" << std::endl;
			break;
	}

	/*std::vector<GLuint> indices;
	for (uint32_t i = 0; i < strandCount; ++i)
	{
		for (uint32_t j = 0; j < particlesPerStrand - 1; ++j)
		{
			const GLuint first = i * particlesPerStrand + j;
			indices.emplace_back(first);
			indices.emplace_back(first + 1);
		}
	}*/

	firsts.reserve(maximumStrandCount);
	lasts.reserve(maximumStrandCount);
	for (uint32_t i = 0; i < maximumStrandCount-1; ++i)
	{
		firsts.push_back(particlesPerStrand * i);
		lasts.push_back(particlesPerStrand);
	}


	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo);

	// Velocities
	data.clear();
	data.reserve(maximumStrandCount * 3);
	for (uint32_t i = 0; i < maximumStrandCount * 3; ++i)
		data.push_back(0.f);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityArrayBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, velocityArrayBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL_NONE);
}

void Hair::setGravity(float strength)
{
	gravity = strength;
	settingsChanged = true;
}

void Hair::incrementStrandCount()
{
	if (strandCount >= maximumStrandCount)
		return;

	strandCount++;
	GLuint localWorkGroupCountX = computeShader.getLocalWorkGroupsCount().x;
	globalWorkGroupCount = strandCount / localWorkGroupCountX;
	if (strandCount % localWorkGroupCountX != 0)
		globalWorkGroupCount += 1;

	computeShader.setGlobalWorkGroupCount(globalWorkGroupCount);
	std::cout << strandCount << '\n';
}

void Hair::decrementStrandCount()
{
	if (strandCount == 0) 
		return;

	--strandCount;
	GLuint localWorkGroupCountX = computeShader.getLocalWorkGroupsCount().x;
	globalWorkGroupCount = strandCount / localWorkGroupCountX;
	if (strandCount % localWorkGroupCountX != 0)
		globalWorkGroupCount += 1;

	computeShader.setGlobalWorkGroupCount(globalWorkGroupCount);
	std::cout << strandCount << '\n';
}

void Hair::setWind(const glm::vec3& direction, float strength)
{
	wind = glm::vec4(direction.x, direction.y, direction.z, strength);
	settingsChanged = true;
}

void Hair::draw() const 
{
	glBindVertexArray(vao);
	glMultiDrawArrays(GL_LINE_STRIP, firsts.data(), lasts.data(), strandCount);
	glBindVertexArray(GL_NONE);
}

void Hair::applyPhysics(float deltaTime, float runningTime)
{
	computeShader.use();
	if (settingsChanged)
	{
		computeShader.setFloat("force.gravity", gravity);
		computeShader.setVec4("force.wind", wind);
		settingsChanged = false;
	}

	computeShader.setUint("hairData.strandCount", strandCount);
	computeShader.setFloat("deltaTime", deltaTime);
	computeShader.setFloat("runningTime", runningTime);
	computeShader.dispatch();
}
