#pragma once
#include "Plane.h"
#include "Camera.h"
const std::string dudvMapPath = "res/Textures/WaterRes/waterDUDV.png";
const std::string norMapPath = "res/Textures/WaterRes/normal.png";
class Water :public Plane
{
	
public:
	Water(std::string shaderName,double width, double depth);
	Water();
	~Water();
	void render();
	void BindFBO_Reflection() { 
	glBindFramebuffer(GL_FRAMEBUFFER, fboReflection); 
	glViewport(0, 0, Camera::Window_Width, Camera::Window_Height);
	}
	void BindFBO_Refraction() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboRefraction);
	glViewport(0, 0, Camera::Window_Width, Camera::Window_Height);
	}
	GLuint getTEX_Reflection() { return reflectionTexture; }
	GLuint getTEX_Refraction() { return refractionTexture; }
	
private :
	Texture dudvMap = Texture(dudvMapPath);
	Texture waterNormalMap = Texture(norMapPath);

	void init();
	GLuint reflectionTexture;
	GLuint refractionTexture;
	GLuint fboReflection;
	GLuint fboRefraction;
	GLuint depthReflection;
	GLuint depthRefraction;

};

