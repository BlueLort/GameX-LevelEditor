#include "Plane.h"

std::string Plane::diffPath="";
std::string Plane::specPath="";
std::string Plane::normPath="";

void Plane::init()
{
	unsigned int vCount = _n * _m;
	unsigned int fCount = 2 * (_n - 1) * (_m - 1);
	_vertices.resize(vCount);
	_indices.resize(3 * fCount);
	//generate vertices
	if (RANDOM_TERRAIN) {
		genRndPlane();
	}
	else {
		genPlaneHeightMap();
	}

	//indicies
	unsigned int k = 0;
	for (unsigned int i = 0; i < _m-1; ++i)
	{
		for (unsigned int j = 0; j < _n-1; ++j)
		{
			_indices[k] = i * _n + j;
			_indices[k + 1] = i * _n + j + 1;
			_indices[k + 2] = (i + 1)*_n + j;
			_indices[k + 3] = (i + 1)*_n + j;
			_indices[k + 4] = i * _n + j + 1;
			_indices[k + 5] = (i + 1)*_n + j + 1;
			//calculate tangents
			calculateTangents(&_vertices[_indices[k]], &_vertices[_indices[k + 1]], &_vertices[_indices[k + 2]]);
			calculateTangents(&_vertices[_indices[k + 3]], &_vertices[_indices[k + 4]], &_vertices[_indices[k + 5]]);

			k += 6; // next quad
		
		}
		
	}
	
	uploadToBuffers();
	//_hMapValues.clear();
}


Plane::Plane(std::string shaderName,float MaxH,float width, float depth,unsigned int n, unsigned int m)
	:_maxHeight(MaxH),_width(width),_depth(depth),_n(n),_m(m)
{
	rgen = new RandomGenerator(_maxHeight);
	RANDOM_TERRAIN = true;
	preInit(shaderName);


}


Plane::Plane(std::string shaderName, float MaxH, float width, float depth, unsigned int n, unsigned int m, std::string hMap)
	:_maxHeight(MaxH), _width(width), _depth(depth), _n(n), _m(m)
{
	RANDOM_TERRAIN = false;
	_hMapValues=IOManager::getInstance()->readImage(hMap, _hMapW, _hMapH, _hMapNrChannels);
	preInit(shaderName);
	hMapPath = hMap;//For Saving purposes
}

Plane::Plane()
{
}


Plane::~Plane()
{
	if (rgen != nullptr) {
		delete rgen;
		rgen = nullptr;
	}
	_hMapValues.clear();
	_vertices.clear();
	_indices.clear();
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_EBO);


}

void Plane::uploadToBuffers()
{

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glGenBuffers(1, &_EBO);
	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);

	// reserving buffer memory
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

	//reserving indices buffer memory
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex,position));
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

void Plane::preInit(std::string shaderName)
{
	diffTex = new Texture(diffPath);
	normalTex = new Texture(normPath);
	specTex = new Texture(specPath);
	this->shader = Shader(("Shaders/" + shaderName + ".vs").c_str(), ("Shaders/" + shaderName + ".frag").c_str(), nullptr);
	this->shader.use();
	this->shader.setInt("material.texture_diffuse", 0);
	this->shader.setInt("material.texture_normal", 1);
	this->shader.setInt("material.texture_specular", 2);
	this->shader.setInt("shadowDirMap", 3);
	Shader::Stop();
	init();
}

void Plane::destroy()
{
	
	shader.destroy();
	diffTex->destroy();
	normalTex->destroy();
	specTex->destroy();
	delete diffTex;
	delete normalTex;
	delete specTex;
}

void Plane::render()
{
	shader.use();
	glm::mat4 model;
	shader.setInt("fX", _n);
	shader.setInt("fZ", _m);
	shader.update(Camera::getInstance()->getProjMatrix(), Camera::getInstance()->getViewMatrix(), model, Camera::getInstance()->Position, pLights, dirLight);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffTex->getTexture());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalTex->getTexture());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, specTex->getTexture());
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Shader::depthMap);
		

		glBindVertexArray(_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT,0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		Shader::Stop();
}

void Plane::renderSimplified()
{
	
	glBindVertexArray(_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}


float Plane::getHeightMapValue(int x, int z)
{
	//static const int HEIGHT_FACTOR = (int)(_maxHeight * (_width / _hMapW) + _maxHeight * (_depth / _hMapH)) / 2;
	Color col = _hMapValues.find(std::pair<int, int>(x, z))->second;
	//VALUE between +_maxHeight/2 & -_maxHeight/2
	return((float)((col.r + col.g + col.b) / (3.0f * 255.0f))*2.0f-1.0f)*_maxHeight/2.0f;
}



void Plane::genRndPlane()
{

	float dx = _width / (_n - 1);
	float dz = _depth / (_m - 1);
	
	rgen->setOffsets(dx, dz);
	float halfWidth = 0.5f*_width;
	float halfDepth = 0.5f*_depth;
	
	///full image over all terrain
	///FACTOR * _n || FACTOR * _n to divide texture between tiles
	float du =1.0f / (_n - 1);
	float dv =1.0f / (_m - 1);
	float x,y, z;
	
	for (unsigned int i = 0; i < _m; ++i)
	{
		 z = halfDepth - i * dz;
		for (unsigned int j = 0; j < _n; ++j)
		{
			x = -halfWidth + j * dx;
			
			y = rgen->getHeight(x, z);
			
			_vertices[i*_n + j].position = glm::vec3(x,y,z);

			//used for lighting.
			glm::vec3 NORMAL;
			NORMAL = getNormalRGEN(x, z, dx, dz);
			_vertices[i*_n + j].normal = NORMAL;


			// used for texturing.
			_vertices[i*_n + j].texCoords = glm::vec2(j * du, i * dv);
		}
	}

}

void Plane::genPlaneHeightMap()
{
	_hmap_DX = _hMapW / _width;
	_hmap_DZ = _hMapH / _depth;
	//SUITABLE ARRAY INDEX
	_hMapW--;
	_hMapH--;
	

	float dx = _width / (_n - 1);
	float dz = _depth / (_m - 1);
	float halfWidth = 0.5f*_width;
	float halfDepth = 0.5f*_depth;
	///full image over all terrain
	///FACTOR * _n || FACTOR * _n to divide texture between tiles
	float du = 1.0f / (_n - 1);
	float dv = 1.0f / (_m - 1);
	float x, y, z;

	for (unsigned int i = 0; i < _m; ++i)
	{
		z = halfDepth - i * dz;
		for (unsigned int j = 0; j < _n; ++j)
		{
			x = -halfWidth + j * dx;
			//better approach is to Lerp(linear interpolate) between 2 height map numbers if the _n&_m >height map width & depth
			//but im keeping it simple here
			int mappedX = map((int)((x)* _hmap_DX), (int)(-halfWidth * _hmap_DX), (int)(halfWidth* _hmap_DX), 0, (int)_hMapW);
			int mappedZ = map((int)((z)* _hmap_DZ), (int)(halfDepth* _hmap_DZ), (int)(-halfDepth * _hmap_DZ), 0, (int)_hMapH);

			y = getHeightMapValue(mappedX, mappedZ);
			_vertices[i*_n + j].position = glm::vec3(x, y, z);

			//used for lighting.
			glm::vec3 NORMAL;
			NORMAL = getNormal((float)mappedX, (float)mappedZ, dx, dz);
			
			_vertices[i*_n + j].normal = NORMAL;


			// used for texturing.
			_vertices[i*_n + j].texCoords = glm::vec2(j * du, i * dv);
		}
	}


}

glm::vec3 Plane::getNormalRGEN(float x, float z, float offsetX, float offsetZ)
{
	float heightL, heightR, heightD, heightU;
	heightL = rgen->getHeight(x - offsetX, z);
	heightR = rgen->getHeight(x + offsetX, z);
	heightD = rgen->getHeight(x, z - offsetZ);
	heightU = rgen->getHeight(x, z + offsetZ);
	glm::vec3 Normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
	return glm::normalize(Normal);
}

glm::vec3 Plane::getNormal(float x,float z,float offsetX,float offsetZ)
{
	float heightL, heightR, heightD, heightU;
		if (x - offsetX<_hMapW || x + offsetX>_hMapW || z - offsetZ<_hMapH || z + offsetZ>_hMapH) {
			return glm::vec3(0, 1, 0);
		}
		heightL = getHeightMapValue((int)((x - offsetX)* _hmap_DX), (int)(z* _hmap_DZ));
		heightR = getHeightMapValue((int)((x + offsetX)* _hmap_DX), (int)(z* _hmap_DZ));
		heightD = getHeightMapValue((int)(x* _hmap_DX), (int)((z - offsetZ )* _hmap_DZ));
		heightU = getHeightMapValue((int)(x* _hmap_DX), (int)((z + offsetZ)* _hmap_DZ));
	glm::vec3 Normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
	return glm::normalize(Normal);
}