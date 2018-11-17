#include "LightLamp.h"

int LightLamp::numOfLamps = 0;

LightLamp::LightLamp(PointLight* pLight)
{
	_width = 0.5f;
	_height = 0.5f;
	_depth = 0.5f;
	name = "Lamp_" + std::to_string(numOfLamps);
	type = "Lamp";
	initShader("Default/Lamp");
	init();
	numOfLamps++;
	Coll = new AABBCollider(position, glm::vec3(_width, _height, _depth));
	pointLight = pLight;
}


LightLamp::~LightLamp()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void LightLamp::draw()
{
	shader.use();
	glm::mat4 model;
	prepareModelMat(model);
	shader.setMat4("projection", Camera::getInstance()->getProjMatrix());
	shader.setMat4("view", Camera::getInstance()->getViewMatrix());
	
	shader.setMat4("model", model);
	shader.setVec3("col", pointLight->diffuse);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	Shader::Stop();

}

void LightLamp::draw(Shader * sh, glm::mat4 model)
{
	sh->use();

	sh->setMat4("projection", Camera::getInstance()->getProjMatrix());
	sh->setMat4("view", Camera::getInstance()->getViewMatrix());
	
	sh->setMat4("model", model);
	sh->setVec3("col", pointLight->diffuse);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
	Shader::Stop();
}

void LightLamp::drawSimplified()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
}

void LightLamp::update(float DeltaTime)
{

	Coll->setPosition(position);
	pointLight->position = position;

}

std::string LightLamp::getInfo()
{
	return "<Light Name=\"" + name + "\" Type=" + type + "\n "
		+ "posX=" + getString(position.x) + " posY=" + getString(position.y) + " posZ=" + getString(position.z) + "\n "	
		+ "constant=" + getString(pointLight->constant) + " linear=" + getString(pointLight->linear) + " quadratic=" + getString(pointLight->quadratic) + "\n "
		+ "ambR=" + getString(pointLight->ambient.r) + " ambG=" + getString(pointLight->ambient.g) + " ambB=" + getString(pointLight->ambient.b) + "\n "
		+ "diffR=" + getString(pointLight->diffuse.x) + " diffG=" + getString(pointLight->diffuse.g) + " diffB=" + getString(pointLight->diffuse.b) + "\n "
		+ "specR=" + getString(pointLight->specular.x) + " specG=" + getString(pointLight->specular.y) + " specB=" + getString(pointLight->specular.b) + " />\n\n";
}

