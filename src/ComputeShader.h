#pragma once
#include "Shader.h"

class ComputeShader : public Shader {
public:
	ComputeShader(const std::string& shaderFile);
	~ComputeShader() override = default;
	void dispatch() const;
	glm::ivec3 getMaxLocalWorkGroups() const;
	glm::ivec3 getLocalWorkGroupsCount() const;
	glm::ivec3 getMaxGlobalWorkGroups() const;
	GLuint getMaxWorkGroupInvocations() const;
	void setGlobalWorkGroupCount(GLuint workGroupX = 1, GLuint workGroupY = 1, GLuint workGroupZ = 1);

private:
	GLuint globalWorkGroupX = 1;
	GLuint globalWorkGroupY = 1;
	GLuint globalWorkGroupZ = 1;
};