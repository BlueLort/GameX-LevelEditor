#pragma once
#include <string.h>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Structs.h"
#include "Camera.h"
#include "PhysicalObject.h"
#include "AABBCollider.h"

const std::string texBox = "res/Textures/Default/COLOR_WHITE.png";
const std::string specBox = "res/Textures/Default/NO_SPECMAP.png";//DEFAULT SPEC MAP
const std::string normalBox = "res/Textures/Default/NO_NORMALMAP.png";//DEFAULT NORMAL MAP
class Box :public PhysicalObject
{
public:
	Box(float width, float depth, float height, std::string shaderName, bool hasGeoShader = false, std::string texPath = texBox, std::string normalPath=normalBox, std::string specMap = specBox);
	Box(float width, float depth, float height, std::string vShader, std::string fShader, std::string gShader, std::string texPath = texBox, std::string normalPath = normalBox, std::string specMap = specBox);
	~Box();
	Box() { init(); };
	 void draw() override;
	 void draw(Shader * sh, glm::mat4 model) override;
	 void drawSimplified() override;
	 void update(float DeltaTime) override;
	 static int numOfBoxes;
	 std::string getInfo() override;
	
protected:

	
	float _width=1;
	float _depth=1;
	float _height=1;
	//int nVerts = 0;//Total number of elements to draw
	std::vector<Vertex> _vertices;

	virtual void init() override;
	virtual void uploadToBuffers() override;

};

