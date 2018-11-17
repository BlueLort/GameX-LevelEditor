#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include "Utility.h"
#include "Camera.h"
class Shader
{
public:
	//general uniforms
	static float deltaTime;
	static int SCR_WIDTH;
	static int SCR_HEIGHT;
	static float farSpace;
	static float nearSpace;
	//Shadow Variable (Easily Accessed by anyClass) -> For now <- Should Change Design Later.
	static GLuint depthMapFBO;
	static GLuint depthMap;
	static Shader* shadowSh;

	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	// activate the shader
	// ------------------------------------------------------------------------
	void use();
	// deactivate the shader
	// ------------------------------------------------------------------------
	static void Stop();
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) /*const*/;
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) /*const*/;
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) /*const*/;
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const glm::vec2 &value) /*const*/;
	void setVec2(const std::string &name, float x, float y) /*const*/;
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value) /*const*/;
	void setVec3(const std::string &name, float x, float y, float z) /*const*/;
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value) /*const*/;
	void setVec4(const std::string &name, float x, float y, float z, float w);
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat) /*const*/;
	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 &mat) /*const*/;
	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat) /*const */;
	//---------------------------------------------------------------------
	//updates the shader with known Shader Values Such as projection view light pos.
	void update(glm::mat4 proj, glm::mat4 view, glm::mat4 model, glm::vec3 camPos, std::vector<PointLight*> _pLights, DirLight _dirLight) ;
	//---------------------------------------------------------------------
	void destroy();

	std::string getVertShName() { return vertexShaderName; }
	std::string getFragShName() { return fragmentShaderName; }
	std::string getGeoShName() { return geoShaderName; }

	Shader(){}
	~Shader();
private:

	GLuint ID;
	std::map<std::string, GLuint> locationsMap;
	GLuint getLoc(const std::string &name);
	std::string vertexShaderName = "NONE";
	std::string fragmentShaderName = "NONE";
	std::string geoShaderName = "NONE";
	void checkCompileErrors(GLuint shader, std::string type);


};

