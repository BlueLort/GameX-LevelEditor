#include "Texture.h"


std::map<std::string, unsigned int> Texture::cachedTextures;

Texture::Texture(std::string filePath)
{
	textureFilePath = filePath;
	std::map<std::string,unsigned int>::iterator ite = cachedTextures.find(filePath);
	if (ite != cachedTextures.end()) {
		texture = ite->second;
		return;
	}
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// load and generate the texture
	unsigned char *data = stbi_load(filePath.c_str(), &_width, &_height, &_nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (_nrChannels == 1)
			format = GL_RED;
		else if (_nrChannels == 3)
			format = GL_RGB;
		else if (_nrChannels == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0,format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	cachedTextures.insert(std::pair<std::string,unsigned int>(filePath, texture));
}

Texture::Texture()
{
}


Texture::~Texture()
{
}

void Texture::destroy()
{
	
	std::map<std::string, unsigned int>::iterator ite = cachedTextures.find(textureFilePath);
	if (ite != cachedTextures.end()) {
		cachedTextures.erase(ite);
	}
	glDeleteTextures(1, &texture);
}

unsigned int Texture::getTexture()
{
	return texture;
}

int Texture::getHeight()
{
	return _height;
}

int Texture::getWidth()
{
	return _width;
}

int Texture::getNR_Channels()
{
	return _nrChannels;
}

GLuint Texture::loadCubemap(std::vector<std::string> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i].c_str() << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
