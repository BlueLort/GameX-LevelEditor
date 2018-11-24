#pragma once
#include "PhysicalObject.h"
#include "Mesh.h"
#include "AABBCollider.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <thread>

class Model : public PhysicalObject
{
public:
	/*  Functions   */
	Model(std::string path, std::string texPath, std::string modelName, std::string shaderName, bool hasGeoShader=false);
	Model(std::string path, std::string texPath, std::string modelName, std::string vShader, std::string fShader, std::string gShader);
	void draw() override;
	void draw(Shader * sh, glm::mat4 model) override;
	void drawSimplified() override;
	void update(float DeltaTime) override;
	std::string getInfo()override;
	
	~Model();
	Model() {};//avoid header declaration error
	static int numOfModels;
private:
	
	/*  Model Data  */
	std::vector<Mesh> meshes;
	std::string loadPathTextures;
	
	/*  Functions   */
	void init() override {};//EMPTY
	void uploadToBuffers() override {};//EMPTY
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName);
};