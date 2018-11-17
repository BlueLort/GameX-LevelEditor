#pragma once
#include <iostream>
#include <string.h>
#include <vector>
#include <GL/glew.h>
#include "stb_image.h"
#include <map>
class Texture
{
	
public:
	Texture(std::string filePath );
	Texture();
	~Texture();
	void destroy();
	unsigned int getTexture();
	int getHeight();
	int getWidth();
	int getNR_Channels();
	std::string getFilePath() { return textureFilePath; }
	std::string getType() { return type; }
	void setType(std::string texType) {  type=texType; }
	static GLuint loadCubemap(std::vector<std::string> faces);
private:
	int _width, _height, _nrChannels;
	std::string type;
	unsigned int texture;
	std::string textureFilePath;
	static std::map<std::string, unsigned int> cachedTextures;
};

