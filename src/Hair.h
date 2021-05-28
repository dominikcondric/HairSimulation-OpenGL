#pragma once
#include "Entity.h"
#include "ComputeShader.h"
#include <memory>
#include <vector>
#include "Sphere.h"

class Hair : public Entity {
public:
	Hair(uint32_t _strandCount = 5000U, float hairLength = 3.f, float hairCurliness = 0.0f);
	~Hair();
	void draw() const override;
	void applyPhysics(float deltaTime, float runningTime);
	void setGravity(float strength);
	void increaseStrandCount();
	void decreaseStrandCount();
	float getCurlRadius() const { return curlRadius; }
	float getFrictionFactor() const { return frictionFactor; }
	uint32_t getParticlesPerStrand() const { return particlesPerStrand; }
	const Entity& getBody() const { return *headModel; }
	
	// Increases curl radius by 0.01 clamped in range [0, 0.05]
	void increaseCurlRadius();

	// Decreases curl radius by 0.01 clamped in range [0, 0.05]
	void decreaseCurlRadius();

	/*
	* Sets wind direction and strength. 
	* If direction set to { 0.f, 0.f, 0.f } wind is dynamic with specified strength
	* Wind strength is clamped in range [0, 1]
	*/
	void setWind(const glm::vec3& direction, float strength);
	
	// Sets friction factor clamped in range [0, 1] 
	void setFrictionFactor(float friction);

private:
	GLuint velocityArrayBuffer = GL_NONE;		// Shader storage buffer object for velocities
	GLuint volumeDensities = GL_NONE;
	GLuint volumeVelocities = GL_NONE;

	std::unique_ptr<Sphere> headModel;
	std::vector<GLint> firsts;
	std::vector<GLint> lasts;
	uint32_t strandCount;
	float curlRadius = 0.0f;
	ComputeShader computeShader;
	uint32_t particlesPerStrand = 15;
	glm::vec4 wind{ 0.f, 0.f, 0.f, 0.2f };
	float gravity = -9.81f;
	bool settingsChanged = false;
	const uint32_t maximumStrandCount = 30000;
	float frictionFactor = 0.07f;
	void constructModel();
	float strandWidth = 1.f;
	float hairLength = 1.f;
};