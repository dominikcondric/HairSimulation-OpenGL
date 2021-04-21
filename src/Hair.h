#pragma once
#include "Entity.h"
#include "ComputeShader.h"

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
	void incrementStrandCount();
	void decrementStrandCount();

	/*
	* Sets wind direction and strength. 
	* If direction set to { 0.f, 0.f, 0.f } wind is dynamic with specified strength
	*/
	void setWind(const glm::vec3& direction, float strength);

private:
	GLuint velocityArrayBuffer = GL_NONE;		// Shader storage buffer object for velocities

	std::vector<GLint> firsts;
	std::vector<GLint> lasts;
	uint32_t strandCount;
	ComputeShader computeShader;						
	uint32_t globalWorkGroupCount = 0;
	GLuint particlesPerStrand = 0;
	glm::vec4 wind{ 0.f, 0.f, 0.f, 0.1f };
	float gravity = -9.81f;
	bool settingsChanged = false;
	const uint32_t maximumStrandCount = 3e5;
};