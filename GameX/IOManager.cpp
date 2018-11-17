#include "IOManager.h"
IOManager* IOManager::IOManagerInstance=new IOManager();


IOManager::IOManager()
{
}

IOManager* IOManager::getInstance()
{
	return IOManagerInstance;
}

std::map<std::pair<int, int>, Color> IOManager::readImage(std::string filePath, int & width, int & height, int & nrChannels)
{
	std::map<std::pair<int, int>, Color> imageMap;
	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels,0);
	if (data)
	{
		unsigned bytePerPixel = nrChannels;
		unsigned char* pixelOffset; 
		unsigned char r, g, b, a;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j <width; j++) {
				pixelOffset = data + (i*width +  j ) * bytePerPixel;
				r = pixelOffset[0];
				g = pixelOffset[1];
				b = pixelOffset[2];
				a = nrChannels >= 4 ? pixelOffset[3] : 0xff;
				imageMap.insert(std::pair<std::pair<int,int>,Color>(std::pair<int,int>(j,i),Color(r,g,b,a)));
			
			}
		}
	}
	else {
		std::cout << "COULDN'T READ Image" << std::endl;
	}
	return imageMap;
}

void IOManager::saveHeightMapImage(std::string filePath, std::map<std::pair<float, float>, float> hMap, float width, float depth, int _m, int _n, float maxHeight)
{
	float dx = width / (_n - 1);
	float dz = depth / (_m - 1);

	float halfWidth = 0.5f*width;
	float halfDepth = 0.5f*depth;
	float* hMapBuffer=new float[_m*_n];
	float X, Y, Z;
	for (unsigned int i = 0; i < _m; ++i)
	{
		Z = halfDepth - i * dz;
		for (unsigned int j = 0; j < _n; ++j)
		{
			X = -halfWidth + j * dx;

			Y = hMap.find(std::pair<float, float>(X, Z))->second;
			hMapBuffer[i*_m + j] = Y + maxHeight / 2;//value from 0 to maxheight
			hMapBuffer[i*_m + j] /= maxHeight;//value from 0 to 1
			hMapBuffer[i*_m + j] *= 255; //value from 0 to 255
		}
	}
	//out as bmp extension
	std::fstream oFile;
	unsigned char *img = NULL;
	int filesize = 54 + 3 * _m*_n; 

	img = new unsigned char[3 * _m*_n];
	int l = 0;
	int x, y, j, i, r, g, b;
	int tmp = 0;
	for (i = 0; i<_m; i++)
	{
		for (j = 0; j<_n; j++)
		{

			r = hMapBuffer[i*_m	+ j];
			g = hMapBuffer[i*_m + j];
			b = hMapBuffer[i*_m + j];
			l++;
			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			x = i; y = (_m - 1) - j;
			img[(x + y * _n) * 3 + 2] = (unsigned char)(r);
			img[(x + y * _n) * 3 + 1] = (unsigned char)(g);
			img[(x + y * _n) * 3 + 0] = (unsigned char)(b);
		}
	}

	unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
	unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
	unsigned char bmppad[3] = { 0,0,0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[4] = (unsigned char)(_n);
	bmpinfoheader[5] = (unsigned char)(_n >> 8);
	bmpinfoheader[6] = (unsigned char)(_n >> 16);
	bmpinfoheader[7] = (unsigned char)(_n >> 24);
	bmpinfoheader[8] = (unsigned char)(_m);
	bmpinfoheader[9] = (unsigned char)(_m >> 8);
	bmpinfoheader[10] = (unsigned char)(_m >> 16);
	bmpinfoheader[11] = (unsigned char)(_m >> 24);

	oFile=std::fstream(filePath, std::ios::out | std::ios::binary);
	if (!oFile.is_open()) {
		std::cout << "Couldn't Save HeightMap" << std::endl;
		return;
	}
	oFile.write((char*)bmpfileheader,14);
	oFile.write((char*)bmpinfoheader, 40);
	for (int i = 0; i<_m; i++)
	{
		oFile.write((char*)img + (_n*(_m - i - 1) * 3) , _n*3);
	//	oFile.write((char*)img + (_n*(_m - i - 1) * 3)+1, _n);
	//	oFile.write((char*)img + (_n*(_m - i - 1) * 3)+2, _n);
		oFile.write((char*)bmppad, (4 - (_n * 3) % 4) % 4);
	}
	oFile.close();

	delete img;
	delete hMapBuffer;


}

void IOManager::loadObjects(std::vector<PhysicalObject*>& gameObjs, std::string file, std::map<std::string, std::string> modelCmb,  TerrainInfo& terrainInfo)
{
	std::string loadFile = "";
	std::string line;
	std::ifstream ifile(file);
	if (ifile.is_open())
	{
		while (getline(ifile, line))
		{
			loadFile += line;
		}
		ifile.close();
	}
	else {
		std::cout << " ERROR ON OPENING \"" + file << "\"!!\n";
		return;
	}
	//LOAD GENERAL SETTINGS
	std::regex reg = std::regex("<General SCR_WIDTH=(.*?) SCR_HEIGHT=(.*?) FOV=(.*?) zNear=(.*?) zFar=(.*?) />");
	std::smatch match;
	std::regex_search(loadFile, match, reg);
	Camera::getInstance()->Window_Width =std::stof(match.str(1));
	Camera::getInstance()->Window_Height = std::stof(match.str(2));
	Camera::getInstance()->Zoom = std::stof(match.str(3));
	Camera::getInstance()->zNear= std::stof(match.str(4));
	Camera::getInstance()->zFar = std::stof(match.str(5));
	loadModels(gameObjs, loadFile,modelCmb);
	loadPhysicalObjects(gameObjs, loadFile);
	loadLights(gameObjs, loadFile);
	terrainInfo = loadTerrain(loadFile);
	
	


}

TerrainInfo IOManager::loadTerrain(std::string loadData)
{
	std::regex reg = std::regex("<Terrain width=(.*?) depth=(.*?) maxHeight=(.*?) heightMap=(.*?) />");
	std::smatch match;
	if (std::regex_search(loadData, match, reg)) {
		return TerrainInfo(glm::vec3(std::stof(match.str(1)), std::stof(match.str(2)), std::stof(match.str(3))),match.str(4));
	}
	else {
		return TerrainInfo(glm::vec3(-1,-1,-1),"");
	}
	
}



IOManager::~IOManager()
{
}

void IOManager::loadModels(std::vector<PhysicalObject*>& gameObjs, std::string loadData,  std::map<std::string, std::string> modelCmb)
{
	//seaarch for models like spheres/boxes
	std::regex reg = std::regex("<LoadedModel Name=\"(.*?)\" Type=(.*?) vertexSh=\"(.*?)\" fragmentSh=\"(.*?)\" geometrySh=\"(.*?)\" posX=(.*?) posY=(.*?) posZ=(.*?) rotX=(.*?) rotY=(.*?) rotZ=(.*?) scalX=(.*?) scalY=(.*?) scalZ=(.*?) />");
	std::smatch match;
	PhysicalObject* p=nullptr;
	
	while (std::regex_search(loadData, match, reg)) {
		
		if (modelCmb.find(match.str(2)) != modelCmb.end()) {
			std::string fPath = modelCmb.find(match.str(2))->second;
			p = new Model(fPath + match.str(2), fPath, match.str(2), match.str(3), match.str(4), match.str(5));
			p->setPos(glm::vec3(std::stof(match.str(6)), std::stof(match.str(7)), std::stof(match.str(8))));
			p->setRotModel(glm::vec3(std::stof(match.str(9)), std::stof(match.str(10)), std::stof(match.str(11))));
			p->setScalModel(glm::vec3(std::stof(match.str(12)), std::stof(match.str(13)), std::stof(match.str(14))));
			p->setName(match.str(1));
			gameObjs.push_back(p);
		}
		else {
			//if user saved a level and config file was changed
			std::cout << "\"" + match.str(2) + "\"" << " IS NOT CONFIGURED IN THE CONFIG FILE!!";
			
		}
		
		loadData = match.suffix().str();
	}
	
}

void IOManager::loadLights(std::vector<PhysicalObject*>& gameObjs, std::string loadData)
{
	//seaarch for PointLights
	std::regex reg = std::regex("<Light Name=\"(.*?)\" Type=(.*?) posX=(.*?) posY=(.*?) posZ=(.*?) constant=(.*?) linear=(.*?) quadratic=(.*?) ambR=(.*?) ambG=(.*?) ambB=(.*?) diffR=(.*?) diffG=(.*?) diffB=(.*?) specR=(.*?) specG=(.*?) specB=(.*?) />");
	std::smatch match;
	PhysicalObject* p = nullptr;
	while (std::regex_search(loadData, match, reg)) {

		PointLight* pL=new PointLight();
		pL->constant = std::stof(match.str(6));
		pL->linear = std::stof(match.str(7));
		pL->quadratic= std::stof(match.str(8));
		pL->ambient = glm::vec3(std::stof(match.str(9)), std::stof(match.str(10)), std::stof(match.str(11)));
		pL->diffuse = glm::vec3(std::stof(match.str(12)), std::stof(match.str(13)), std::stof(match.str(14)));
		pL->specular = glm::vec3(std::stof(match.str(15)), std::stof(match.str(16)), std::stof(match.str(17)));
		p = new LightLamp(pL);
		p->setPos(glm::vec3(std::stof(match.str(3)), std::stof(match.str(4)), std::stof(match.str(5))));
		p->setName(match.str(1));
		gameObjs.push_back(p);
		loadData = match.suffix().str();
	}
}

void IOManager::loadPhysicalObjects(std::vector<PhysicalObject*>& gameObjs, std::string loadData)
{
	//seaarch for loaded models
	std::regex  reg = std::regex("<PhysicalObject Name=\"(.*?)\" Type=(.*?) diffTex=\"(.*?)\" normTex=\"(.*?)\" specTex=\"(.*?)\" vertexSh=\"(.*?)\" fragmentSh=\"(.*?)\" geometrySh=\"(.*?)\" posX=(.*?) posY=(.*?) posZ=(.*?) rotX=(.*?) rotY=(.*?) rotZ=(.*?) scalX=(.*?) scalY=(.*?) scalZ=(.*?) />");
	std::smatch match;
	PhysicalObject* p = nullptr;
	while (std::regex_search(loadData, match, reg)) {
		
		if (match.str(2) == "Sphere") {//SPHERE
			p = new Sphere(1.0f,30.0f, 30.0f, match.str(6), match.str(7), match.str(8), match.str(3), match.str(4), match.str(5));
	
		}
		else {//BOX
			p = new Box(1.0f, 1.0f, 1.0f, match.str(6), match.str(7), match.str(8), match.str(3), match.str(4), match.str(5));
		}
		p->setPos(glm::vec3(std::stof(match.str(9)), std::stof(match.str(10)), std::stof(match.str(11))));
		p->setRotModel(glm::vec3(std::stof(match.str(12)), std::stof(match.str(13)), std::stof(match.str(14))));
		p->setScalModel(glm::vec3(std::stof(match.str(15)), std::stof(match.str(16)), std::stof(match.str(17))));
		p->setName(match.str(1));
		gameObjs.push_back(p);
		loadData = match.suffix().str();
	}
	
}
