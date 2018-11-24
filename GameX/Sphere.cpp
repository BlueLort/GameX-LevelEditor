#include "Sphere.h"


int Sphere::numOfSpheres = 0;
Sphere::Sphere(float radius, int slices, int rings, std::string shaderName, bool hasGeoShader, std::string texPath , std::string normalPath , std::string specMap ) :
	_radius(radius), _slices(slices), _rings(rings)
{
	name = "Sphere_" + std::to_string(numOfSpheres);
	type = "Sphere";
	Tex = new Texture(texPath);
	normalTex = new Texture(normalPath);
	SpecTex = new Texture(specMap);
	initShader(shaderName, hasGeoShader);
	init();
	numOfSpheres++;
	Coll = new SphereCollider(position, radius);
}
Sphere::Sphere(float radius, int slices, int rings, std::string vShader, std::string fShader, std::string gShader, std::string texPath, std::string normalPath, std::string specMap):
	_radius(radius), _slices(slices), _rings(rings)
	{
		name = "Sphere_" + std::to_string(numOfSpheres);
		type = "Sphere";
		Tex = new Texture(texPath);
		normalTex = new Texture(normalPath);
		SpecTex = new Texture(specMap);

		this->shader = Shader(vShader.c_str(), fShader.c_str(), gShader!="NONE" ? gShader.c_str() : nullptr);
		this->shader.use();
		this->shader.setInt("material.texture_diffuse", 0);
		this->shader.setInt("material.texture_normal", 1);
		this->shader.setInt("material.texture_specular", 2);
		this->shader.setInt("shadowDirMap", 3);
		Shader::Stop();
		init();
		numOfSpheres++;
		Coll = new SphereCollider(position, radius);
}
//GENERATING USING SPHERE COORDINATES
void Sphere::init()
{
	//float dRadius = _radius / _n;
	/*
	NO TOP 
		/   /    /    /
	  /   /   /    /    /
	/   /   /   /   /    /
	  /   /   /   /   /
	   /   /   /    /
	NO BOT
	*/
	float x, y, z;
	float dPhi, dTheta;
	dPhi = (float)(( FULL_PI) / _rings);
	dTheta = (float)((2* FULL_PI) / _slices);
	float du = 1.0f / (_slices);
	float dv = 1.0f / (_rings);
	//unsigned int vCount = (_rings+1) * (_slices+1);
	unsigned int fCount = 2 * (_rings) * (_slices);
	//_vertices.resize(vCount);
	_indices.resize(3 * fCount);
	for ( int i = 0; i <= _rings; i++) {
		y =_radius*cosf(i*dPhi);

		for ( int j = 0; j <= _slices; j++) {
			Vertex newVert;
			x = _radius * sinf(i*dPhi)*cosf(j*dTheta);
			z = _radius * sinf(i*dPhi)*sinf(j*dTheta);
			
			//set Pos
			newVert.position = glm::vec3(x, y, z);
			//set normal
			//Setting normal is easy as i draw it at origin
			newVert.normal = glm::normalize(glm::vec3(x, y, z));
			newVert.texCoords = glm::vec2(j*du, i*dv);
			_vertices.push_back(newVert);
		}
	}
	//indicies
	unsigned int k = 0;
	for (int i = 0; i < _rings ; ++i)
	{
		for (int j = 0; j < _slices ; ++j)
		{
			//add one because we duplicate the first and last vertex per ring
			_indices[k] = i * (_slices+1) + j;
			_indices[k + 1] = i * (_slices+1) + j+1;
			_indices[k + 2] = (i + 1)*(_slices+1) + j;
			_indices[k + 3] = (i + 1)*(_slices+1) + j;
			_indices[k + 4] = i * (_slices+1) + j + 1;
			_indices[k + 5] = (i+1) * (_slices+1) + j+1 ;
			//calculate tangents
			calculateTangents(&_vertices[_indices[k]], &_vertices[_indices[k + 1]], &_vertices[_indices[k + 2]]);
			calculateTangents(&_vertices[_indices[k + 3]], &_vertices[_indices[k + 4]], &_vertices[_indices[k + 5]]);
			k += 6; // next quad 
		
		}

	}

	uploadToBuffers();
	/*
	nVerts = _indices.size();
	_vertices.clear();
	*/
	//
}


void Sphere::draw()
{
	shader.use();
	glm::mat4 model;
	prepareModelMat(model);
	shader.update(Camera::getInstance()->getProjMatrix(), Camera::getInstance()->getViewMatrix(), model, Camera::getInstance()->Position,  pLights, dirLight);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Tex->getTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTex->getTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, SpecTex->getTexture());
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, _indices.size() , GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D,0);
	
	glBindVertexArray(0);
	Shader::Stop();
}
void Sphere::draw(Shader * sh, glm::mat4 model)
{
	sh->use();

	sh->update(Camera::getInstance()->getProjMatrix(), Camera::getInstance()->getViewMatrix(), model, Camera::getInstance()->Position, pLights, dirLight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Tex->getTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTex->getTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, SpecTex->getTexture());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Shader::depthMap);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
	Shader::Stop();
}
void Sphere::drawSimplified()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
void Sphere::update(float DeltaTime)
{

	Coll->setPosition(position);

}

inline std::string Sphere::getInfo() {
		return "<PhysicalObject Name=\"" + name + "\" Type=" + type + "\n "
			+ "diffTex=\"" + Tex->getFilePath() + "\"\n normTex=\"" + normalTex->getFilePath() + "\"\n specTex=\"" + SpecTex->getFilePath() + "\"\n "
			+ "vertexSh=\"" + shader.getVertShName() + "\" fragmentSh=\"" + shader.getFragShName() + "\" geometrySh=\"" + shader.getGeoShName() + "\"\n "
			+ "posX=" + getString(position.x) + " posY=" + getString(position.y) + " posZ=" + getString(position.z) + "\n "
			+ "rotX=" + getString(rotation.x) + " rotY=" + getString(rotation.y) + " rotZ=" + getString(rotation.z) + "\n "
			+ "scalX=" + getString(scale.x) + " scalY=" + getString(scale.y) + " scalZ=" + getString(scale.z) + " />\n\n";
	
}


Sphere::~Sphere()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Sphere::uploadToBuffers()
{

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// reserving buffer memory
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

	//reserving indices buffer memory
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

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