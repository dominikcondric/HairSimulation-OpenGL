#pragma once
#include "Entity.h"
#include "ComputeShader.h"
#include <memory>
#include <vector>
#include <array>
#include "Sphere.h"
#include "Window.h"

class Hair : public Entity {
public:
	Hair(uint32_t _strandCount = 5000U, float hairLength = 3.f, float hairCurliness = 0.0f);
	~Hair();
	void draw() const override;
	void drawHead() const;
	void applyPhysics(float deltaTime, float runningTime);
	void setGravity(float strength);
	void increaseStrandCount();
	void decreaseStrandCount();
	void increaseVelocityDamping();
	void decreaseVelocityDamping();
	float getCurlRadius() const { return curlRadius; }
	float getFrictionFactor() const { return frictionFactor; }
	uint32_t getParticlesPerStrand() const { return particlesPerStrand; }
	const std::array<std::unique_ptr<Sphere>, 7>& getEllipsoids() const { return ellipsoids; }
	
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

	uint32_t strandCount;
	float curlRadius = 0.0f;
	ComputeShader computeShader;
	uint32_t particlesPerStrand = 15;
	glm::vec4 wind{ 0.f, 0.f, 0.f, 0.2f };
	float gravity = -9.81f;
	bool settingsChanged = false;
	const uint32_t maximumStrandCount = 30000U;
	float frictionFactor = 0.02f;
	void constructModel();
	float strandWidth = 0.2f;
	float hairLength = 1.f;
	float velocityDampingCoefficient = 0.9f;

	// Head variables
	glm::vec3 headColor;
	GLuint headVbo = GL_NONE;
	GLuint headVao = GL_NONE;
	GLuint headEbo = GL_NONE;
	uint32_t indexCount = 0;
	std::array<std::unique_ptr<Sphere>, 7> ellipsoids;
	float ellipsoidsRadius = 0.5f;
};