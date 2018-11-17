#pragma once
#include <string.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Texture.h"
#include "Collider.h"
#include "Utility.h"


//ABSTRACT OBJECT CLASS
class PhysicalObject
{
public:
	
	virtual void draw()=0;
	virtual void draw(Shader * sh, glm::mat4 model) = 0;
	virtual void drawSimplified() = 0;//just render using the current used shader(MEANT FOR SHADOWS)
	virtual void update(float DeltaTime)=0;
	//helper functions
	void initShader(std::string shaderName,bool haveGeoShader=false) {
		this->shader = Shader(("Shaders/" + shaderName + ".vs").c_str(), ("Shaders/" + shaderName + ".frag").c_str(),haveGeoShader? ("Shaders/" + shaderName + ".geo").c_str():nullptr);
		this->shader.use();
		this->shader.setInt("material.texture_diffuse", 0);
		this->shader.setInt("material.texture_normal", 1);
		this->shader.setInt("material.texture_specular", 2);
		this->shader.setInt("shadowDirMap", 3);

		Shader::Stop();
	}
	void prepareModelMat(glm::mat4& modelMat) {
		modelMat = glm::translate(modelMat, position);

		modelMat = glm::rotate(modelMat, glm::radians(rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(-rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
		
		//IDK WHAT AXIS SHOULD I ROTATE ABOUT HERE
		modelMat = glm::rotate(modelMat, glm::radians(rotation.z), glm::vec3(1.0f, 1.0f, 0.0f));
		modelMat = glm::scale(modelMat, scale);
	}


	//GETTERS
	std::string getName() { return name; }
	std::string getType() { return type; }
	Collider* getCollider(){ return Coll; }
	Shader* getShader() { return &shader; }
	glm::vec3 getPos() { return position; }
	glm::vec3 getRotModel() { return rotation; }
	glm::vec3 getScalModel() { return scale; }

	//SETTERS
	void setName(std::string newName) {name=newName; }
	void setType(std::string newType) { type = newType; }
	void setPos(glm::vec3 newPos) {
		position=newPos;
		Coll->setPosition(newPos);
	}
	void setRotModel(glm::vec3 newRotModel) {  rotation = newRotModel; }
	void setScalModel(glm::vec3 newScalModel) {
		scale = newScalModel;
		Coll->setScale(newScalModel);
	}
	//for saving purposes
	virtual std::string getInfo() = 0;
	//toString
	std::string toString() { return "Name: " + name + " ,Type: " + type; }
	void destroy() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		shader.destroy();
		if (Tex != nullptr) {
			Tex->destroy();
			delete Tex;
		}
		if (normalTex != nullptr) {
			normalTex->destroy();
			delete normalTex;
		}
		if (SpecTex != nullptr) {
			SpecTex->destroy();
			delete SpecTex;
		}
		delete Coll;
	}
protected:
	GLuint VAO;
	GLuint EBO;
	GLuint VBO;

	std::string name;
	std::string type;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	Shader shader;
	Texture* Tex=nullptr;
	Texture* normalTex = nullptr;
	Texture* SpecTex = nullptr;
	Collider* Coll = nullptr;

	virtual void init() = 0;
	virtual void uploadToBuffers() = 0;

private:
	

};

