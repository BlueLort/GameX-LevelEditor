#pragma once
#include <string.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "Structs.h"
#include "Camera.h"
#include "PhysicalObject.h"
#include "SphereCollider.h"
//TODO
//I THINK THERE IS A BUG WITH CREATING AS TOP AND BOT VERT CREATED MORE THAN ONCE 


const std::string texSph = "res/Textures/Default/COLOR_WHITE.png";
const std::string specSph= "res/Textures/Default/NO_SPECMAP.png";//DEFAULT SPEC MAP
const std::string normalSph= "res/Textures/Default/NO_NORMALMAP.png";//DEFAULT NORMAL MAP
class Sphere : public PhysicalObject
{
public:

	Sphere(float radius, int slices, int rings, std::string shaderName, bool hasGeoShader = false, std::string texPath = texSph, std::string normalPath = normalSph, std::string specMap = specSph);
	Sphere(float radius, int slices, int rings, std::string vShader, std::string fShader, std::string gShader, std::string texPath = texSph, std::string normalPath = normalSph, std::string specMap = specSph);
	 void draw() override;
	 void draw(Shader * sh, glm::mat4 model) override;
	 void drawSimplified() override;
	 void update(float DeltaTime) override;
	 std::string getInfo()override;
	 
	 Sphere() { init(); };
	 ~Sphere();
	 static int numOfSpheres;

	 inline const std::vector<Vertex>& getVertices() const { return _vertices; }
	 inline const std::vector<unsigned int>& getIndices() const { return _indices; }

private:

	
	float _radius=0.25f;
	 int _rings=35;
	 int _slices=35;
	 int nVerts = 0;//Total number of elements to draw
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;
	
	void init() override;
	void uploadToBuffers() override;

	
};

