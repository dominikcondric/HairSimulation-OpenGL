#pragma once
#include "Entity.h"
#include "ComputeShader.h"
#include <vector>

class Hair : public Entity {
public:
	enum class HairType {
		Straight,
		Curly
	};

public:
	Hair(uint32_t _strandCount = 5000U, uint32_t strandSegmentsCount = 10U, HairType type = HairType::Straight);
	~Hair();
	void draw() const override;
	void applyPhysics(float deltaTime, float runningTime);
	void constructModel(HairType type);
	void setGravity(float strength);
	void increaseStrandCount();
	void decreaseStrandCount();

	/*
	* Sets wind direction and strength. 
	* If direction set to { 0.f, 0.f, 0.f } wind is dynamic with specified strength
	* Wind strength is clamped in range [0, 1]
	*/
	void setWind(const glm::vec3& direction, float strength);
	
	/*
	* Sets friction factor clamped in range [0, 1] 
	*/
	void setFrictionFactor(float friction);
	float getFrictionFactor() const { return frictionFactor; }

private:
	GLuint velocityArrayBuffer = GL_NONE;		// Shader storage buffer object for velocities
	GLuint volumeDensities = GL_NONE;
	GLuint volumeVelocities = GL_NONE;

	std::vector<GLint> firsts;
	std::vector<GLint> lasts;
	uint32_t strandCount;
	ComputeShader computeShader;
	GLuint particlesPerStrand = 0;
	glm::vec4 wind{ 0.f, 0.f, 0.f, 0.7f };
	float gravity = -9.81f;
	bool settingsChanged = false;
	const int maximumStrandCount = 3e4;
	float frictionFactor = 0.1f;
};