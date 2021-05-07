#include "Hair.h"
#include <iostream>
#include <glm/gtc/random.hpp>

Hair::Hair(uint32_t _strandCount, uint32_t strandSegmentsCount, HairType type) : strandCount(_strandCount), computeShader("HairComputeShader.glsl")
{
	strandSegmentsCount = glm::clamp<uint32_t>(strandSegmentsCount, 2U, 49U);
	particlesPerStrand = strandSegmentsCount + 1;
	constructModel(type);
	GLuint localWorkGroupCountX = computeShader.getLocalWorkGroupsCount().x;
	GLuint globalWorkGroupCount = strandCount / localWorkGroupCountX;
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
	computeShader.setFloat("frictionCoefficient", frictionFactor);
}

Hair::~Hair()
{
	glDeleteBuffers(1, &velocityArrayBuffer);
	glDeleteBuffers(1, &volumeDensities);
	glDeleteBuffers(1, &volumeVelocities);
}

void Hair::constructModel(HairType type)
{
	const float segmentLength = 1.f / (particlesPerStrand - 1);
	std::vector<float> data;
	data.reserve(maximumStrandCount * particlesPerStrand * 3);

	switch (type)
	{
	case Hair::HairType::Straight:
		for (uint32_t i = 0; i < maximumStrandCount; ++i)
		{
			const glm::vec2 randomDiskCoordinates = glm::diskRand(0.5f);
			for (uint32_t j = 0; j < particlesPerStrand; ++j)
			{
				data.push_back(randomDiskCoordinates.x);
				data.push_back(0.f - segmentLength * j);
				data.push_back(randomDiskCoordinates.y);
			}
		}
		break;

	case Hair::HairType::Curly:
		std::cout << "Not yet implemented!" << std::endl;
		break;
	}

	firsts.reserve(maximumStrandCount);
	lasts.reserve(maximumStrandCount);
	for (uint32_t i = 0; i < maximumStrandCount - 1; ++i)
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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);

	// Velocities
	data.clear();
	data.reserve(maximumStrandCount * particlesPerStrand * 3);
	for (uint32_t i = 0; i < maximumStrandCount * particlesPerStrand * 3; ++i)
		data.push_back(0.f);

	glGenBuffers(1, &velocityArrayBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityArrayBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityArrayBuffer);

	float voxelGridSize = 5 * 5 * 5 * sizeof(float); // 5x5x5 volume, 3 floats per vertex
	glGenBuffers(1, &volumeDensities);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeDensities);
	glBufferData(GL_SHADER_STORAGE_BUFFER, voxelGridSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, volumeDensities);

	voxelGridSize *= 3.f;
	glGenBuffers(1, &volumeVelocities);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeVelocities);
	glBufferData(GL_SHADER_STORAGE_BUFFER, voxelGridSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, volumeVelocities);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL_NONE);
}

void Hair::setGravity(float strength)
{
	gravity = strength;
	settingsChanged = true;
}

void Hair::increaseStrandCount()
{
	strandCount = glm::clamp<int>(strandCount + 10, 0, maximumStrandCount);

	GLuint localWorkGroupCountX = computeShader.getLocalWorkGroupsCount().x;
	GLuint globalWorkGroupCount = strandCount / localWorkGroupCountX;
	if (strandCount % localWorkGroupCountX != 0)
		globalWorkGroupCount += 1;

	computeShader.setGlobalWorkGroupCount(globalWorkGroupCount);
	std::cout << "Strand count: " << strandCount << '\n';
}

void Hair::decreaseStrandCount()
{
	strandCount = glm::clamp<int>(strandCount - 10, 0, maximumStrandCount);

	GLuint localWorkGroupCountX = computeShader.getLocalWorkGroupsCount().x;
	GLuint globalWorkGroupCount = strandCount / localWorkGroupCountX;
	if (strandCount % localWorkGroupCountX != 0)
		globalWorkGroupCount += 1;

	computeShader.setGlobalWorkGroupCount(globalWorkGroupCount);
	std::cout << "Strand count: " << strandCount << '\n';
}

void Hair::setWind(const glm::vec3& direction, float strength)
{
	wind = glm::vec4(direction.x, direction.y, direction.z, glm::clamp(strength, 0.f, 1.f));
	settingsChanged = true;
}

void Hair::setFrictionFactor(float friction)
{
	frictionFactor = glm::clamp<float>(friction, 0.f, 1.f);
	settingsChanged = true;
	std::cout << "Friction factor: " << frictionFactor << std::endl;
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
		computeShader.setFloat("frictionCoefficient", frictionFactor);
		settingsChanged = false;
	}

	computeShader.setUint("hairData.strandCount", strandCount);
	computeShader.setFloat("deltaTime", deltaTime);
	computeShader.setFloat("runningTime", runningTime);
	computeShader.setUint("state", 0);
	computeShader.dispatch();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	computeShader.setUint("state", 1);
	computeShader.dispatch();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	computeShader.setUint("state", 2);
	computeShader.dispatch();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeDensities);
	float* densities = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	for (int i = 0; i < 5 * 5 * 5; ++i)
		densities[i] = 0.f;

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeVelocities);
	float* velocities = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

	for (int i = 0; i < 5 * 5 * 5 * 3; ++i)
		velocities[i] = 0.f;

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL_NONE);
}
