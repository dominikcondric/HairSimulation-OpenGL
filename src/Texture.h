#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
	Texture(const std::string& texName, GLuint texType = GL_TEXTURE_2D, const bool gammaCorrection = true);
	~Texture();
	void activateAndBind(const GLuint textureUnit) const;

private:
	GLuint textureID{ 0 };
	const GLuint textureType;
	void generateCubeMap(const std::string& name, const bool gammaCorrection) const;
	void generate2DMap(const std::string& name, const bool gammaCorrection) const;
};
