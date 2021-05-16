#include "DrawingShader.h"

DrawingShader::DrawingShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	compileAndAttachShader(vertexShaderFile, vertexShaderID);
	compileAndAttachShader(fragmentShaderFile, fragmentShaderID);
	linkProgram();
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

DrawingShader::DrawingShader(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile)
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	compileAndAttachShader(vertexShaderFile, vertexShaderID);
	compileAndAttachShader(fragmentShaderFile, fragmentShaderID);
	compileAndAttachShader(geometryShaderFile, geometryShaderID);
	linkProgram();
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteShader(geometryShaderID);
}
