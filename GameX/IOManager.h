#pragma once
#include <iostream>
#include "stb_image.h"
#include <map>
#include "Structs.h"
#include "PhysicalObject.h"
#include "Model.h"
#include "Box.h"
#include "Sphere.h"
#include "LightLamp.h"
#include <regex>
#include "Camera.h"
class IOManager
{
public:
	static IOManager* getInstance();
	std::map<std::pair<int, int>, Color> readImage(std::string filePath, int& width, int& height, int& nrChannels);
	void saveHeightMapImage(std::string filePath, std::map<std::pair<float, float>, float> hMap, float width, float depth, int _m, int _n,float maxHeight);
	void loadObjects(std::vector<PhysicalObject*>& gameObjs,std::string file,std::map<std::string,std::string> modelCmb,TerrainInfo& terrainInfo);
	
	~IOManager();
private :
	static IOManager* IOManagerInstance;
	void loadModels(std::vector<PhysicalObject*>& gameObjs, std::string loadData, std::map<std::string, std::string> modelCmb);
	void loadLights(std::vector<PhysicalObject*>& gameObjs, std::string loadData);
	void loadPhysicalObjects(std::vector<PhysicalObject*>& gameObjs, std::string loadData);
	TerrainInfo loadTerrain(std::string loadData);
	

	IOManager();


};

