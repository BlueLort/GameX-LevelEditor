#include "Water.h"


void Water::init()
{
	float halfWidth = 0.5f*_width;
	float halfDepth = 0.5f*_depth;

	_vertices.resize(4);
	_indices.resize(6);
	//its just a 2D Quad rely heavily on shaders
	_vertices[0]=(Vertex(-halfWidth, 0, halfDepth, 0,1, 0,0, 1, 0, 0, 0, 0, 0,0));  //TOP LEFT
	_vertices[1]=(Vertex(halfWidth, 0, halfDepth, 0, 1, 0, 1,1, 0, 0, 0, 0, 0, 0));	// TOP RIGHT
	_vertices[2]=(Vertex(halfWidth, 0, -halfDepth, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0));	//BOT RIGHT
	_vertices[3]=(Vertex(-halfWidth, 0, -halfDepth, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0)); //BOT LEFT
	
	_indices[0] =0;
	_indices[ 1] =1;
	_indices[ 2] = 2;
	_indices[ 3] = 2;
	_indices[ 4] = 3;
	_indices[ 5] = 0;
	
	


	uploadToBuffers();
	_vertices.clear();
}


Water::Water(std::string shaderName,double width, double depth) {
	this->shader = Shader(("Shaders/" + shaderName + ".vs").c_str(), ("Shaders/" + shaderName + ".frag").c_str(), nullptr);
	this->shader.use();
	this->shader.setVec3("viewPos",Camera::getInstance()->Position);
	this->shader.setInt("reflectionTexture", 0);
	this->shader.setInt("refractionTexture", 1);
	this->shader.setInt("depthReflection", 2);
	this->shader.setInt("depthRefraction", 3);
	this->shader.setInt("dudvMap", 4);
	this->shader.setInt("normalMap", 5);
	Shader::Stop();

	_width = width;
	_depth = depth;
	//n = 2 , m = 2 as its rely heavily on shaders
	 _n = 2;
	 _m = 2; 
	
	///////
	///////
	/*                                                        FBO REFLECTION                                  */
	glGenFramebuffers(1, &fboReflection);
	glBindFramebuffer(GL_FRAMEBUFFER, fboReflection);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//COLOR TEXTURE for REFLECTION ATTACHMENT
	glGenTextures(1, &reflectionTexture);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Camera::Window_Width, Camera::Window_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	

	//COLOR TEX ATTACH
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	//depth REFLECTION
	glGenTextures(1, &depthReflection);
	glBindTexture(GL_TEXTURE_2D, depthReflection);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Camera::Window_Width, Camera::Window_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	//depth WITH DEPTH AND STENCIL ATTACH
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthReflection,0);
	glBindTexture(GL_TEXTURE_2D, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer REFLECTION is not complete!" << std::endl;
	/*                                                   FBO REFRACTION                                        */
	glGenFramebuffers(1, &fboRefraction);
	glBindFramebuffer(GL_FRAMEBUFFER, fboRefraction);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//COLOR TEXTURE for REFRACTION ATTACHMENT
	glGenTextures(1, &refractionTexture);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Camera::Window_Width, Camera::Window_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//COLOR TEX ATTACH
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//depth REFRACTION
	glGenTextures(1, &depthRefraction);
	glBindTexture(GL_TEXTURE_2D, depthRefraction);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, Camera::Window_Width, Camera::Window_Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//depth WITH DEPTH AND STENCIL ATTACH
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRefraction,0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer REFRACTION is not complete!" << std::endl;



	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	init();
	
}

Water::Water()
{
}


Water::~Water()
{
	
	_vertices.clear();
	_indices.clear();
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_EBO);
	glDeleteTextures(1, &reflectionTexture);
	glDeleteTextures(1, &refractionTexture);
	glDeleteTextures(1, &depthReflection);
	glDeleteTextures(1, &depthRefraction);
	glDeleteFramebuffers(1, &fboReflection);
	glDeleteFramebuffers(1, &fboRefraction);
}

void Water::render()
{
	shader.use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 model;
	shader.update(Camera::getInstance()->getProjMatrix(), Camera::getInstance()->getViewMatrix(), model,Camera::getInstance()->Position, pLights, dirLight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthReflection);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthRefraction);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, dudvMap.getTexture());
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D,waterNormalMap.getTexture());
	glBindVertexArray(_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	
	glDisable(GL_BLEND);
	Shader::Stop();
}
