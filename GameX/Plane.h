#pragma once
#include <vector>
#include <string.h>
#include <GL/glew.h>
#include "Structs.h"
#include "Texture.h"
#include "Camera.h"
#include "RandomGenerator.h"
#include "Utility.h"
#include "IOManager.h"



class Plane
{
public:

	static std::string diffPath;
	static std::string specPath;
	static std::string normPath;
	Plane(std::string shaderName, float MaxH,float width,float depth, unsigned int n, unsigned int m);
	Plane(std::string shaderName, float MaxH, float width, float depth, unsigned int n, unsigned int m,std::string hMap);
	Plane();
	~Plane();		
	float getMaxHeight() { return _maxHeight; }
	float getWidth() { return _width; }
	float getDepth() { return _depth; }
	std::vector<Vertex> getVertices() { return _vertices; }
	std::vector<unsigned int> getIndices() { return _indices; }
	Shader* getShader() { return &shader; }
	std::string getHeightMapPath() { return hMapPath; }
	RandomGenerator* getRGen() { return rgen; }

	void setMaxHeight(float newHeight) { _maxHeight = newHeight; }
	void setWidth(float newWidth) { _width = newWidth; }
	void setDepth(float newDepth) { _depth = newDepth; }
	void setHeightMapPath(std::string newPath) {hMapPath = newPath;}


	virtual void destroy();
	virtual void render();
	virtual void renderSimplified();
protected:
    float _maxHeight=30.0;
	float _width;
	float _depth;
	Shader shader;
	unsigned int _n;
	unsigned int _m;
	float du, dv;
	GLuint _VBO;
	GLuint _VAO;
	GLuint _EBO;
	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;
	virtual void init();
	void uploadToBuffers();
private:

	bool RANDOM_TERRAIN=false;
	void preInit(std::string shaderName);
	double _hmap_DX;
	double _hmap_DZ;
	std::string hMapPath;
	Texture* diffTex;
	Texture* normalTex;
	Texture* specTex;
	int _hMapW, _hMapH, _hMapNrChannels;//init them to 1 
	std::string _hMapPath;
	RandomGenerator* rgen=nullptr;//TO PROVIDE RANDOM HEIGHT IF NEEDED
	
	std::map<std::pair<int, int>, Color> _hMapValues;
	
	void genRndPlane();
	void genPlaneHeightMap();
	
	glm::vec3 getNormalRGEN(float x, float z, float offsetX, float offsetZ);
	glm::vec3 getNormal(float x, float z, float offsetX, float offsetZ);
	float getHeightMapValue(int x, int z);
	

};

