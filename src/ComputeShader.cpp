#include "ComputeShader.h"

ComputeShader::ComputeShader(const std::string& shaderFile)
{
	GLuint shaderID = glCreateShader(GL_COMPUTE_SHADER);
	compileAndAttachShader(shaderFile, shaderID);
	linkProgram();
	glDeleteShader(shaderID);
}

void ComputeShader::dispatch() const
{
	glDispatchCompute(globalWorkGroupX, globalWorkGroupY, globalWorkGroupZ);
}

glm::ivec3 ComputeShader::getMaxLocalWorkGroups() const
{
	glm::ivec3 values;
	glGetProgramiv(programID, GL_MAX_COMPUTE_WORK_GROUP_SIZE, &values.x);
	return values;
}

glm::ivec3 ComputeShader::getLocalWorkGroupsCount() const
{
	glm::ivec3 values;
	glGetProgramiv(programID, GL_COMPUTE_WORK_GROUP_SIZE, &values.x);
	return values;
}

glm::ivec3 ComputeShader::getMaxGlobalWorkGroups() const
{
	glm::ivec3 values;
	glGetProgramiv(programID, GL_MAX_COMPUTE_WORK_GROUP_COUNT, &values.x);
	return values;
}

GLuint ComputeShader::getMaxWorkGroupInvocations() const
{
	GLint value;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &value);
	return value;
}

void ComputeShader::setGlobalWorkGroupCount(GLuint workGroupX, GLuint workGroupY, GLuint workGroupZ)
{
	if (workGroupX != 0)
		globalWorkGroupX = workGroupX;

	if (workGroupY != 0)
		globalWorkGroupY = workGroupY;

	if (workGroupZ != 0)
		globalWorkGroupZ = workGroupZ;
}
