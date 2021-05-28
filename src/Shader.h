#pragma once
#include <string>
#include <glad/glad.h>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
	Shader() = default;
	virtual ~Shader() = 0;
	void use() const;
	void setInt(const std::string& name, const int value) const;
	void setIntArray(const std::string& name, GLsizei count, const GLint value[]) const;
	void setUint(const std::string& name, const uint32_t value) const;
	void setUintArray(const std::string& name, int count, const uint32_t value[]) const;
	void setBool(const std::string& name, const bool value) const;
	void setBoolArray(const std::string& name, GLsizei count, bool values[]) const;
	void setFloat(const std::string& name, const float value) const;
	void setFloatArray(const std::string& name, GLsizei count, const GLfloat values[]) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2Array(const std::string& name, GLsizei count, const glm::vec2 values[]) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3Array(const std::string& name, GLsizei count, const glm::vec3 values[]) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4Array(const std::string& name, GLsizei count, const glm::vec4 values[]) const;
	void setMat2(const std::string& name, const glm::mat2& value) const;
	void setMat2Array(const std::string& name, GLsizei count, const glm::mat2 values[]) const;
	void setMat3(const std::string& name, const glm::mat3& value) const;
	void setMat3Array(const std::string& name, GLsizei count, const glm::mat3 values[]) const;
	void setMat4(const std::string& name, const glm::mat4& value) const;
	void setMat4Array(const std::string& name, GLsizei count, const glm::mat4 values[]) const;
	void bindShaderUboToBindingPoint(const std::string& uniformBlockName, const GLuint bindingPoint) const;

protected:
	GLuint programID = GL_NONE;
	mutable std::unordered_map<std::string, std::pair<GLint, bool>> uniformCache;
	GLint getUniformLocation(const std::string& name) const;
	void linkProgram() const;
	void compileAndAttachShader(const std::string& shaderFileName, GLuint& shaderID);
};