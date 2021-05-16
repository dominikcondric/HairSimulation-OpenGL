#pragma once
#include "Shader.h"

class DrawingShader : public Shader {
public:
	DrawingShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
	DrawingShader(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile);
	~DrawingShader() override = default;
};