#include "Box.h"



int Box::numOfBoxes = 0;

Box::Box(float width, float depth, float height, std::string shaderName, bool hasGeoShader , std::string texPath, std::string normalPath, std::string specMap) :
	_width(width), _depth(depth), _height(height)
{
	
	name = "Box_" + std::to_string(numOfBoxes);
	type = "Box";
	Tex = new Texture(texPath);
	normalTex= new Texture(normalPath);
	SpecTex = new Texture(specMap);
	initShader(shaderName, hasGeoShader);
	init();
	numOfBoxes++;
	Coll = new AABBCollider(position, glm::vec3(width,height,depth));
}

Box::Box(float width, float depth, float height, std::string vShader, std::string fShader, std::string gShader, std::string texPath, std::string normalPath, std::string specMap):
_width(width), _depth(depth), _height(height)
{
	name = "Box_" + std::to_string(numOfBoxes);
	type = "Box";
	Tex = new Texture(texPath);
	normalTex = new Texture(normalPath);
	SpecTex = new Texture(specMap);
	this->shader = Shader(vShader.c_str(), fShader.c_str(), gShader != "NONE" ? gShader.c_str() : nullptr);
	this->shader.use();
	this->shader.setInt("material.texture_diffuse", 0);
	this->shader.setInt("material.texture_normal", 1);
	this->shader.setInt("material.texture_specular", 2);
	this->shader.setInt("shadowDirMap", 3);
	Shader::Stop();
	init();
	numOfBoxes++;
	Coll = new AABBCollider(position, glm::vec3(width, height, depth));
	
}

Box::~Box()
{
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Box::draw()
{
	shader.use();
	glm::mat4 model;
	prepareModelMat(model);
	shader.update(Camera::getInstance()->getProjMatrix(), Camera::getInstance()->getViewMatrix(), model, Camera::getInstance()->Position,pLights,dirLight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Tex->getTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTex->getTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, SpecTex->getTexture());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Shader::depthMap);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindVertexArray(0);
	Shader::Stop();

}

void Box::draw(Shader * sh, glm::mat4 model)
{
	sh->use();

	sh->update(Camera::getInstance()->getProjMatrix(), Camera::getInstance()->getViewMatrix(), model, Camera::getInstance()->Position, pLights, dirLight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Tex->getTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTex->getTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, SpecTex->getTexture());

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
	Shader::Stop();
}

void Box::drawSimplified()
{

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
}


void Box::update(float DeltaTime)
{


	Coll->setPosition(position);

	//TODO
	//PHYSICS
}

inline std::string Box::getInfo() {
		return "<PhysicalObject Name=\"" + name + "\" Type=" + type + "\n "
			+ "diffTex=\"" + Tex->getFilePath() + "\"\n normTex=\"" + normalTex->getFilePath() + "\"\n specTex=\"" + SpecTex->getFilePath() + "\"\n "
			+ "vertexSh=\"" + shader.getVertShName() + "\" fragmentSh=\"" + shader.getFragShName() + "\" geometrySh=\"" + shader.getGeoShName() + "\"\n "
			+ "posX=" + getString(position.x) + " posY=" + getString(position.y) + " posZ=" + getString(position.z) + "\n "
			+ "rotX=" + getString(rotation.x) + " rotY=" + getString(rotation.y) + " rotZ=" + getString(rotation.z) + "\n "
			+ "scalX=" + getString(scale.x) + " scalY=" + getString(scale.y) + " scalZ=" + getString(scale.z) + " />\n\n";
	
}


void Box::init()
{
	_vertices.resize(36);

	
	//IN CCW(FACE CULLING IS ON)
	//Back 
	_vertices[0] = Vertex(-_width / 2, -_height / 2, -_depth / 2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	_vertices[1] = Vertex(-_width / 2, _height / 2, -_depth / 2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	_vertices[2] = Vertex(_width / 2, _height / 2, -_depth / 2, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);



	_vertices[3] = Vertex(_width / 2, _height / 2, -_depth / 2, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	_vertices[4] = Vertex(_width / 2, -_height / 2, -_depth / 2, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	_vertices[5] = Vertex(-_width / 2, -_height / 2, -_depth / 2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

		//first triangle
		calculateTangents(&_vertices[0], &_vertices[1], &_vertices[2]);
		//second triangle
		calculateTangents(&_vertices[3], &_vertices[4], &_vertices[5]);
	//Front
	_vertices[6] = Vertex(_width / 2, -_height / 2, _depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	_vertices[7] = Vertex(_width / 2, _height / 2, _depth / 2, 0.0f, 0.0f,1.0f, 1.0f, 1.0f);
	_vertices[8] = Vertex(-_width / 2, _height / 2, _depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);

	_vertices[9] = Vertex(-_width / 2, _height / 2, _depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	_vertices[10] = Vertex(-_width / 2, -_height / 2, _depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	_vertices[11] = Vertex(_width / 2, -_height / 2, _depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		//first triangle
		calculateTangents(&_vertices[6], &_vertices[7], &_vertices[8]);
		//second triangle
		calculateTangents(&_vertices[9], &_vertices[10], &_vertices[11]);
	//Left
	_vertices[12] = Vertex(-_width / 2, -_height / 2, _depth / 2, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	_vertices[13] = Vertex(-_width / 2, _height / 2, _depth / 2, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	_vertices[14] = Vertex(-_width / 2, _height / 2, -_depth / 2, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	_vertices[15] = Vertex(-_width / 2, _height / 2, -_depth / 2, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	_vertices[16] = Vertex(-_width / 2, -_height / 2, -_depth / 2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	_vertices[17] = Vertex(-_width / 2, -_height / 2, _depth / 2, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		//first triangle
		calculateTangents(&_vertices[12], &_vertices[13], &_vertices[14]);
		//second triangle
		calculateTangents(&_vertices[15], &_vertices[16], &_vertices[17]);
	//Right
	_vertices[18] = Vertex(_width / 2, -_height / 2, -_depth / 2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	_vertices[19] = Vertex(_width / 2, _height / 2, -_depth / 2, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	_vertices[20] = Vertex(_width / 2, _height / 2, _depth / 2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	
	_vertices[21] = Vertex(_width / 2, _height / 2, _depth / 2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	_vertices[22] = Vertex(_width / 2, -_height / 2, _depth / 2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	_vertices[23] = Vertex(_width / 2, -_height / 2, -_depth / 2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		//first triangle
		calculateTangents(&_vertices[18], &_vertices[19], &_vertices[20]);
		//second triangle
		calculateTangents(&_vertices[21], &_vertices[22], &_vertices[23]);
	//Bottom
	_vertices[24] = Vertex(_width / 2, -_height / 2, _depth / 2, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	_vertices[25] = Vertex(-_width / 2, -_height / 2, _depth / 2, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	_vertices[26] = Vertex(-_width / 2, -_height / 2, -_depth / 2, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	
	_vertices[27] = Vertex(-_width / 2, -_height / 2, -_depth / 2, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	_vertices[28] = Vertex(_width / 2, -_height / 2, -_depth / 2, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	_vertices[29] = Vertex(_width / 2, -_height / 2, _depth / 2, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
		//first triangle
		calculateTangents(&_vertices[24], &_vertices[25], &_vertices[26]);
		//second triangle
		calculateTangents(&_vertices[27], &_vertices[28], &_vertices[29]);

	//Top
	_vertices[30] = Vertex(_width / 2, _height / 2, _depth / 2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	_vertices[31] = Vertex(_width / 2, _height / 2, -_depth / 2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	_vertices[32] = Vertex(-_width / 2, _height / 2, -_depth / 2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	_vertices[33] = Vertex(-_width / 2, _height / 2, -_depth / 2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	_vertices[34] = Vertex(-_width / 2, _height / 2, _depth / 2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	_vertices[35] = Vertex(_width / 2, _height / 2, _depth / 2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
		//first triangle
		calculateTangents(&_vertices[30], &_vertices[31], &_vertices[32]);
		//second triangle
		calculateTangents(&_vertices[33], &_vertices[34], &_vertices[35]);

	

	uploadToBuffers();
	_vertices.clear();

}

void Box::uploadToBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// reserving buffer memory
	glBufferData(GL_ARRAY_BUFFER,_vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, tangentU));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, bitangent));
	glEnableVertexAttribArray(4);

}
