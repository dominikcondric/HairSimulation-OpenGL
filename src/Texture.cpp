#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include "PathConfig.h"

#define CHAR_OFFSET 48U

Texture::Texture(const std::string& texName, GLuint texType, const bool gammaCorrection) : textureType(texType)
{
	glGenTextures(1, &textureID);

	switch (texType)
	{
	case GL_TEXTURE_2D:
		generate2DMap(texName, gammaCorrection);
		break;
	case GL_TEXTURE_CUBE_MAP:
		generateCubeMap(texName, gammaCorrection);
		break;
	}
}

Texture::~Texture()
{
	glDeleteBuffers(1, &textureID);
}

void Texture::activateAndBind(const GLuint textureUnit) const
{
	glActiveTexture(textureUnit);
	glBindTexture(textureType, textureID);
}

void Texture::generateCubeMap(const std::string& name, const bool gammaCorrection) const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	unsigned char* data;
	for (int i = 0; i < 6; i++)
	{
		std::string texName = TEXTURE_FOLDER + name;
		texName.insert(texName.begin() + texName.find('.'), char(i + CHAR_OFFSET));
		data = stbi_load(texName.c_str(), &width, &height, &nrChannels, 0);
		if (nrChannels == 4)
		{
			if (gammaCorrection) 
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else 
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			if (gammaCorrection) 
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			else 
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Texture::generate2DMap(const std::string& name, const bool gammaCorrection) const
{
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load((TEXTURE_FOLDER + name).c_str(), &width, &height, &nrChannels, 0);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (nrChannels == 4)
	{
		if (gammaCorrection) 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		if (gammaCorrection) 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}
