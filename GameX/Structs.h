
#pragma once
#include <iostream>
#include <vector>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define FULL_PI (3.14159265358979)
struct Color {
	unsigned char r, g, b,a;
	Color(unsigned char pr, unsigned char pg, unsigned char pb, unsigned char pa) {
		r = pr;
		g = pg;
		b = pb;
		a = pa;
	}
	Color() {

	}
};
struct TerrainInfo {
	glm::vec3 dimensions;
	std::string filePath;
	TerrainInfo(glm::vec3 dim, std::string path) {
		dimensions = dim;
		filePath = path;
	}
	TerrainInfo() {};
};

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec3 tangentU;
	glm::vec3 bitangent;

	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tu, float tv,
		float tx, float ty, float tz,
		float bx, float by, float bz ) {
		position.x = px;
		position.y = py;
		position.z = pz;
		normal.x = nx;
		normal.y = ny;
		normal.z = nz;
		tangentU.x = tx;
		tangentU.y = ty;
		tangentU.z = tz;
		bitangent.x = bx;
		bitangent.y = by;
		bitangent.z = bz;
		texCoords.x = tu;
		texCoords.y = tv;
	}
	Vertex(
		glm::vec3 p, glm::vec3 n, glm::vec2 t, glm::vec3 tu, glm::vec3 bt) {
		position = p;
		normal = n;
		tangentU = tu;
		bitangent = bt;
		texCoords = t;
	}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tu, float tv) {
		position.x = px;
		position.y = py;
		position.z = pz;
		normal.x = nx;
		normal.y = ny;
		normal.z = nz;
		texCoords.x = tu;
		texCoords.y = tv;
	}
	Vertex() {
	}
};
struct Ray {
	glm::vec3 RO;//Ray Origin
	glm::vec3 RD;//Ray Diriction
	Ray(glm::vec3 _RO, glm::vec3 _RD) :RO(_RO), RD(_RD) {
		
	}

};
struct DirLight {
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct PointLight {
	std::string name;
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};
struct TexturePathSTRUCT {
	std::string diffFile;
	std::string normFile;
	std::string specFile;
};
extern struct DirLight dirLight;
extern std::vector<struct PointLight*> pLights;
