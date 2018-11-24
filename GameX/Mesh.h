#pragma once
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Structs.h"
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <map>
#include "HalfEdge.h"
#include "HENode.h"
#include "HEVertex.h"

class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	/*  Functions  */
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh();
	void Draw(Shader shader);
	void DrawSimplified();
	void destroy();
private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	/*  Functions    */
	void setupMesh();
};