#include "Model.h"


int Model::numOfModels = 0;


/*  Functions   */

 Model::Model(std::string path,std::string texPath,std::string modelName, std::string shaderName,bool hasGeoShader)
{
	 
	loadPathTextures = texPath;
	loadModel(path.c_str());
	initShader(shaderName,hasGeoShader);
	Coll = new AABBCollider(position, glm::vec3(2.0f, 2.0f, 2.0f));
	name = "Model_" + std::to_string(numOfModels);
	type = modelName;//u can change type name using setter
	numOfModels++;
}

 Model::Model(std::string path, std::string texPath, std::string modelName, std::string vShader, std::string fShader, std::string gShader)
 {
	 loadPathTextures = texPath;
	 loadModel(path.c_str());
	 this->shader = Shader(vShader.c_str(), fShader.c_str(), gShader != "NONE" ? gShader.c_str() : nullptr);
	 this->shader.use();
	 this->shader.setInt("material.texture_diffuse", 0);
	 this->shader.setInt("material.texture_normal", 1);
	 this->shader.setInt("material.texture_specular", 2);
	 Shader::Stop();
	 Coll = new AABBCollider(position, glm::vec3(2.0f, 2.0f, 2.0f));
	 name = "Model_" + std::to_string(numOfModels);
	 type = modelName;//u can change type name using setter
	 numOfModels++;
 }

void Model::draw()
{
	shader.use();
	glm::mat4 model;
	prepareModelMat(model);
	shader.update(Camera::getInstance()->getProjMatrix(), Camera::getInstance()->getViewMatrix(), model, Camera::getInstance()->Position, pLights, dirLight);
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	Shader::Stop();
	
}

void Model::draw(Shader * sh,glm::mat4 model)
{
	sh->use();
	sh->update(Camera::getInstance()->getProjMatrix(), Camera::getInstance()->getViewMatrix(), model, Camera::getInstance()->Position, pLights, dirLight);
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(*sh);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Model::drawSimplified()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].DrawSimplified();
}

void Model::update(float DeltaTime)
{

	Coll->setPosition(position);

	//TODO
	//PHYSICS
	//Animations
}

std::string Model::getInfo()
{
	return "<LoadedModel Name=\"" + name + "\" Type=" + type + "\n "
		+ "vertexSh=\"" + shader.getVertShName() + "\" fragmentSh=\"" + shader.getFragShName()+"\" geometrySh=\"" + shader.getGeoShName()+"\"\n "
		+ "posX=" + getString(position.x) + " posY=" + getString(position.y) + " posZ=" + getString(position.z) + "\n "
		+ "rotX=" + getString(rotation.x) + " rotY=" + getString(rotation.y) + " rotZ=" + getString(rotation.z) + "\n "
		+ "scalX=" + getString(scale.x) + " scalY=" + getString(scale.y) + " scalZ=" + getString(scale.z) + " />\n\n";

}


Model::~Model()
{
	for (Mesh m : meshes) {
		m.destroy();
	}
	meshes.clear();

}



void Model::loadModel(std::string path)
{

	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;
		if (mesh->mTangents != nullptr) {
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangentU = vector;
		}

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = loadMaterialTextures(material,
			aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = loadMaterialTextures(material,
			aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
		std::map<std::string, Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			//CACHED TEXTURES
			if (textures.find(std::string(str.C_Str())) == textures.end()) {
				Texture texture(loadPathTextures+std::string(str.C_Str()));
				//std::cout << loadPathTextures + std::string(str.C_Str()) << std::endl;
				texture.setType(typeName);
				textures.insert(std::pair<std::string, Texture>(std::string(str.C_Str()), texture));
				
			}
		}
		std::map<std::string, Texture>::iterator ite = textures.begin();
		std::vector<Texture> allTextures;
		while (ite != textures.end()) {
			allTextures.push_back(ite->second);
			ite++;
		}
		return allTextures;
	
}