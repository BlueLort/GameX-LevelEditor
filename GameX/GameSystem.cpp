#include "GameSystem.h"
GameSystem* GameSystem::gameSystemInstance=new GameSystem();
GameSystem::GameSystem() :_window(nullptr), _gameState(GameState::PLAY)
{

}


GameSystem::~GameSystem()
{
}

void GameSystem::run()
{
	initSystem();
	ASearch();
	gameLoop();
}



void GameSystem::initSystem()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	//ANTI ALIASING
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	glEnable(GL_MULTISAMPLE);
	_window = SDL_CreateWindow("GameX Level Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL);
	if (_window == nullptr) {
		printError("Could'nt Create SDL Window");
	}
	SDL_GLContext glContext = SDL_GL_CreateContext(_window);
	if (glContext == nullptr) {
		printError("Could'nt Create SDL GL Context");
	}
	//Hide the Mouse Cursor
	SDL_ShowCursor(SDL_DISABLE);


	GLenum Error=glewInit();
	if (Error != GLEW_OK) {
		printError("Could'nt Initialize GLEW");
	}
	SDL_GL_SetSwapInterval(1);//Enable VSYNC
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//openGL Settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//

	

	
	initGUI();

	initConfig();
	//Get Input keeper Instance(Singleton)
	_inputKeeper = InputKeeper::getInstance();

	//Get Model Selector Instance(Singleton)
	_modelSelector = ModelSelector::getInstance();

	//Get Camera
	_camera = Camera::getInstance();

	//INIT DIR LIGHT
	
	selectedObjectSh = new Shader("Shaders/Default/SelectedObject.vs", "Shaders/Default/SelectedObject.frag");
	Shader::shadowSh= new Shader("Shaders/Default/Shadow.vs", "Shaders/Default/Shadow.frag");
	skyboxSh = new Shader("Shaders/Default/Skybox/Skybox.vs", "Shaders/Default/Skybox/Skybox.frag");
	skyboxSh->use();
	skyboxSh->setInt("skybox", 0);
	Shader::Stop();
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	prepareShadowFBO();
}

void GameSystem::initGUI()
{
	_GUI.init("res/GUI");
	_GUI.loadScheme("AlfiskoSkin.scheme");
	_GUI.loadScheme("VanillaSkin.scheme");
	_GUI.loadFont("Jura-13");
	_GUI.loadFont("Jura-10");
	
	_GUI.loadLayout("LevelEditor.layout");

	_GUI.setMouseCursor("AlfiskoSkin/MouseArrow");
	_GUI.showMouseCursor();
	//MODEL STATS
	fmw_ModelStats = static_cast<CEGUI::FrameWindow*>(_GUI.getRootWindow()->getChild("fmw_ModelStats"));
	lab_type = static_cast<CEGUI::DefaultWindow*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/lab_type"));
	ebx_name= static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_name"));
	ebx_pos_x = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_pos_x"));
	ebx_pos_y = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_pos_y"));
	ebx_pos_z = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_pos_z"));
	ebx_scal_x = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_scal_x"));
	ebx_scal_y = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_scal_y"));
	ebx_scal_z = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_scal_z"));
	ebx_rot_x = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_rot_x"));
	ebx_rot_y = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_rot_y"));
	ebx_rot_z = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/ebx_rot_z"));
	 but_updateModels = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/but_update"));
	 but_remove = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/but_remove"));
	 but_reset = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_ModelStats/but_reset"));



	 but_updateModels->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		 CEGUI::Event::Subscriber(&GameSystem::updateModelsOnAction, this));
	 but_remove->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		 CEGUI::Event::Subscriber(&GameSystem::removeOnAction, this));
	 but_reset->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		 CEGUI::Event::Subscriber(&GameSystem::resetOnAction, this));
	 //---------------------------
	 //MODEL Editor
	 fmw_ModelEditor = static_cast<CEGUI::FrameWindow*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor"));
	 rdb_2_move =static_cast<CEGUI::RadioButton*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/rdb_2_move"));
	 rdb_2_scale = static_cast<CEGUI::RadioButton*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/rdb_2_scale"));
	 rdb_2_rotate = static_cast<CEGUI::RadioButton*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/rdb_2_rotate"));
	 chb_x = static_cast<CEGUI::ToggleButton*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/chb_x"));
	 chb_y = static_cast<CEGUI::ToggleButton*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/chb_y"));
	 chb_z = static_cast<CEGUI::ToggleButton*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/chb_z"));
	 chb_wfmode = static_cast<CEGUI::ToggleButton*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/chb_wfmode"));
	 chb_geoShader= static_cast<CEGUI::ToggleButton*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/chb_geoShader"));
	 cmb_models = static_cast<CEGUI::Combobox*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/cmb_models"));
	 cmb_shaders = static_cast<CEGUI::Combobox*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/cmb_shaders"));
	 cmb_textures = static_cast<CEGUI::Combobox*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/cmb_textures"));
	 but_placeModels = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_ModelEditor/but_place"));

	 but_placeModels->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		 CEGUI::Event::Subscriber(&GameSystem::placeModelsOnAction, this));

	 cmb_models->addItem(new CEGUI::ListboxTextItem("Sphere"));
	 cmb_models->addItem(new CEGUI::ListboxTextItem("Box"));
	 cmb_shaders->addItem(new CEGUI::ListboxTextItem("Default"));
	 cmb_shaders->addItem(new CEGUI::ListboxTextItem("Default Norm. Mapping"));
	 cmb_textures->addItem(new CEGUI::ListboxTextItem("COLOR_WHITE.png"));
	 cmb_textures->addItem(new CEGUI::ListboxTextItem("COLOR_RED.png"));
	 cmb_textures->addItem(new CEGUI::ListboxTextItem("COLOR_BLACK.png"));
	 cmb_textures->addItem(new CEGUI::ListboxTextItem("COLOR_BLUE.png"));
	 cmb_textures->addItem(new CEGUI::ListboxTextItem("COLOR_GREEN.png"));

	 TexturePathSTRUCT tp;
	 std::string texPath = "res/Textures/Default/";
	 tp.diffFile = texPath + "COLOR_WHITE.png";
	 tp.normFile = texPath + "NO_NORMALMAP.png";
	 tp.specFile = texPath + "NO_SPECMAP.png";
	 comboBox_Textures.insert(std::pair<std::string,TexturePathSTRUCT>("COLOR_WHITE.png",tp));
	 tp.diffFile = texPath + "COLOR_RED.png";
	 comboBox_Textures.insert(std::pair<std::string,TexturePathSTRUCT>("COLOR_RED.png", tp));
	 tp.diffFile = texPath + "COLOR_BLACK.png";
	 comboBox_Textures.insert(std::pair<std::string,TexturePathSTRUCT>("COLOR_BLACK.png", tp));
	 tp.diffFile = texPath + "COLOR_BLUE.png";
	 comboBox_Textures.insert(std::pair<std::string,TexturePathSTRUCT>("COLOR_BLUE.png", tp));
	 tp.diffFile = texPath + "COLOR_GREEN.png";
	 comboBox_Textures.insert(std::pair<std::string,TexturePathSTRUCT>("COLOR_GREEN.png", tp));

	 //---------------------------
	 //---------------------------
	 //Save Load
	 fmw_SaveLoad = static_cast<CEGUI::FrameWindow*>(_GUI.getRootWindow()->getChild("fmw_SaveLoad"));
	 ebx_fileName = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_SaveLoad/ebx_fileName"));
	 but_save = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_SaveLoad/but_save"));
	 but_load = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_SaveLoad/but_load"));
	 but_new = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_SaveLoad/but_new"));

	 but_save->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		 CEGUI::Event::Subscriber(&GameSystem::saveOnAction, this));
	 but_load->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		 CEGUI::Event::Subscriber(&GameSystem::loadOnAction, this));
	 but_new->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		 CEGUI::Event::Subscriber(&GameSystem::newOnAction, this));
	 //------------------------------------
	 //---------------------------
	 //Projections and light
	 fmw_projLights = static_cast<CEGUI::FrameWindow*>(_GUI.getRootWindow()->getChild("fmw_projLights"));
	 ebx_fov= static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_fov"));
	 ebx_near = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_near"));
	 ebx_far = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_far"));
	 ebx_dir_x = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_dir_x"));
	 ebx_dir_y = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_dir_y"));
	 ebx_dir_z = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_dir_z"));
	 but_updatePL = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_projLights/but_update"));
	 ebx_constant = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_constant"));
	 ebx_linear = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_linear"));
	 ebx_quadratic = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_quadratic"));
	 ebx_amb_r= static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_amb_r"));
	 ebx_amb_g = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_amb_g"));
	 ebx_amb_b = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_amb_b"));
	 ebx_diff_r = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_diff_r"));
	 ebx_diff_g = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_diff_g"));
	 ebx_diff_b = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_diff_b"));
	 ebx_spec_r = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_spec_r"));
	 ebx_spec_g = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_spec_g"));
	 ebx_spec_b = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_projLights/ebx_spec_b"));
	 but_placeLight = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_projLights/but_place"));

	 but_updatePL->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		 CEGUI::Event::Subscriber(&GameSystem::updatePLOnAction, this));
	 but_placeLight->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		 CEGUI::Event::Subscriber(&GameSystem::placeLightOnAction, this));

	

	 //------------------------------------
	 //Terrain Settings
	fmw_TerrainSettings = static_cast<CEGUI::FrameWindow*>(_GUI.getRootWindow()->getChild("fmw_TerrainSettings"));
	chb_hideWater= static_cast<CEGUI::ToggleButton*>(_GUI.getRootWindow()->getChild("fmw_TerrainSettings/chb_hideWater"));
	chb_hidePlane= static_cast<CEGUI::ToggleButton*>(_GUI.getRootWindow()->getChild("fmw_TerrainSettings/chb_hidePlane"));
	cmb_heightMaps = static_cast<CEGUI::Combobox*>(_GUI.getRootWindow()->getChild("fmw_TerrainSettings/cmb_heightMaps"));
    but_genPlane = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_TerrainSettings/but_genPlane"));
    but_genRndPlane = static_cast<CEGUI::PushButton*>(_GUI.getRootWindow()->getChild("fmw_TerrainSettings/but_genRndPlane"));
	ebx_width= static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_TerrainSettings/ebx_width"));
	ebx_depth = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_TerrainSettings/ebx_depth"));
	ebx_maxHeight = static_cast<CEGUI::Editbox*>(_GUI.getRootWindow()->getChild("fmw_TerrainSettings/ebx_maxHeight"));

	but_genPlane->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		CEGUI::Event::Subscriber(&GameSystem::genPlaneOnAction, this));
	but_genRndPlane->subscribeEvent(CEGUI::PushButton::EventMouseClick,
		CEGUI::Event::Subscriber(&GameSystem::genRandomPlaneOnAction, this));

	//keep water hidden by default
	chb_hideWater->setSelected(true);
	 //---------------------------


}

void GameSystem::initConfig()
{
	std::string configFile="";
	std::string line;
	std::ifstream ifile("LevelEditor.config");
	if (ifile.is_open())
	{
		while (getline(ifile, line))
		{
			configFile += line;
		}
		ifile.close();
	}
	else {
		std::cout << "Couldnt Open \"LevelEditor.config\" .\nfile is created Check it to config ur file paths ..\n";
		std::ofstream oFile("LevelEditor.config");
		if (oFile.is_open())
		{
			oFile << "#all paths must be config. in RelativePath.\n";
			oFile << "#application will only catch first definition of paths.\n";
			oFile << "#textures of the model should be stored in the folder of model name/Textures .\n";
			oFile << "#for example for model named \"Tree.obj\" -> {Tree/Textures} .\n";
			oFile << "#SaveLoadPath -> path where levels are stored.\n";
			oFile << "#Texture->path where Textures are Stored and its name.\n";
			oFile << "#Model -> put name of the model to load it by auto to the Models combobox (loaded from models path).\n";
			oFile << "#models name should be followed by format like \"Tree.obj\" .\n";
			oFile << "#textures name should be followed by format like \"Tree.jpg\".\n";
			oFile << "#to comment out put any symbol into anytag and it will be considered not read like <#HeightMap ..";
			oFile << "#General Settings(default values).\n";
			oFile << "<PlaneConfig width=70 depth=70 maxHeight=12/>\n";
			oFile << "<PlaneDiff path=\"res/Textures/Plane/desert_mntn_d.jpg\"/>\n";
			oFile << "<PlaneNorm path=\"res/Textures/Plane/desert_mntn_n.jpg\"/>\n";
			oFile << "<PlaneSpec path=\"res/Textures/Plane/desert_mntn_s.jpg\"/>\n";
			oFile << "<#LoadSkybox \n";
			oFile << "xPostive=\"right.jpg\" xNegative=\"left.jpg\" \n";
			oFile << "yPostive=\"top.jpg\" yNegative=\"bottom.jpg\" \n";
			oFile << "zPostive=\"front.jpg\" zNegative=\"back.jpg\" \n";
			oFile << ">res/Textures/Skybox/skybox/</LoadSkybox>";
			oFile << "<ClearColor r=77 g=77 b=255 a=255/>\n";
			oFile << "<Projection FOV=45.0 zNear=0.1 zFar=400.0/>\n";
			oFile << "<DirLight x=-0.2 y=-1.0 z=-0.3 ambR=12.0 ambG=12.0 ambB=12.0 diffR=210.0 diffG=210.0 diffB=210.0 specR=210.0 specG=210.0 specB=210.0/>\n";
			oFile << "<PointLight const=1.0 linear=0.036 quadratic=0.09 ambR=31 ambG=31 ambB=31 diffR=102 diffG=102 diffB=102 specR=128 specG=128 specB=128/>\n";
			oFile << "#-------------------------------------\n";
			oFile << "<HeightMap name=\"aloo\">res/Textures/Plane/</HeightMap>\n";
			oFile << "<SaveLoadPath>res/Levels/</SaveLoadPath>\n";
			oFile << "<Texture name=\"sphTex.png\">res/Textures/Sphere/</Texture>\n";
			oFile << "<Model name=\"nanosuit.obj\">res/Models/nanosuit/</Model>\n";
			oFile << "<Shader name=\"testShader\"/>\n";
			oFile.close();
		}
		else { std::cout << "Unable to Create Config File..\n";
		system("pause");
		exit(-1);
		}
		system("pause");
		initConfig();//recall function to read the file
	}
	std::smatch match;
	std::regex reg= std::regex("<SaveLoadPath>(.*?)</SaveLoadPath>");

	std::regex_search(configFile, match, reg);
	SaveLoadPath = match.str(1);	
	readDefaultSettings(configFile);
	readHMaps(configFile);
	readTexts(configFile);
	readModels(configFile);
	readShaders(configFile);
	

}

void GameSystem::prepareShadowFBO()
{
	glGenFramebuffers(1, &Shader::depthMapFBO);

	glGenTextures(1, &Shader::depthMap);
	glBindTexture(GL_TEXTURE_2D, Shader::depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		Camera::Window_Width, Camera::Window_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, Shader::depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Shader::depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameSystem::readTexts(std::string fData)
{
	std::regex reg = std::regex("<Texture diff=\"(.*?)\" normal=\"(.*?)\" spec=\"(.*?)\">(.*?)</Texture>");
	std::smatch match;
	while (std::regex_search(fData, match, reg)) {

		TexturePathSTRUCT tp;
		std::string texPath = match.str(4);
		tp.diffFile = texPath+match.str(1);
		//if default texture dont add path 
		if (match.str(2) == "default")tp.normFile = "default";
		else tp.normFile = texPath+match.str(2);
		if (match.str(3) == "default")tp.specFile = "default";
		else tp.specFile = texPath+match.str(3);
	
		comboBox_Textures.insert(std::pair<std::string,TexturePathSTRUCT>(match.str(1),tp ));
		cmb_textures->addItem(new CEGUI::ListboxTextItem(match.str(1).c_str()));
		fData = match.suffix().str();
	}

}

void GameSystem::readHMaps(std::string fData)
{
	std::regex reg = std::regex("<HeightMap name=\"(.*?)\">(.*?)</HeightMap>");
	std::smatch match;
	while (std::regex_search(fData, match, reg)) {
		std::string fileName = match.str(1);
		std::string filePath = match.str(2);
		

		comboBox_hMaps.insert(std::pair<std::string, std::string>(fileName,filePath+fileName));
		cmb_heightMaps->addItem(new CEGUI::ListboxTextItem(fileName.c_str()));
		fData = match.suffix().str();
	}
}

void GameSystem::readModels(std::string fData)
{
	std::regex reg = std::regex("<Model name=\"(.*?)\">(.*?)</Model>");
	std::smatch match;
	while (std::regex_search(fData, match, reg)) {
		std::string modName = match.str(1);
		std::string modPath = match.str(2);
		comboBox_Models.insert(std::pair<std::string, std::string>(modName, modPath ));
		cmb_models->addItem(new CEGUI::ListboxTextItem(modName.c_str()));
		fData = match.suffix().str();
	}

}
void GameSystem::readShaders(std::string fData)
{
	std::regex reg = std::regex("<Shader name=\"(.*?)\"/>");
	std::smatch match;
	while (std::regex_search(fData, match, reg)) {
		std::string shName = match.str(1);
		cmb_shaders->addItem(new CEGUI::ListboxTextItem(shName.c_str()));
		fData = match.suffix().str();
	}

}

void GameSystem::readDefaultSettings(std::string fData)
{
	std::regex reg = std::regex("<ClearColor r=(.*?) g=(.*?) b=(.*?) a=(.*?)/>");
	std::smatch match;
	std::regex_search(fData, match, reg);
	clearColor.r = std::stof(match.str(1)) / 255.0f;
	clearColor.g = std::stof(match.str(2)) / 255.0f;
	clearColor.b= std::stof(match.str(3))  / 255.0f;
	clearColor.a = std::stof(match.str(4)) / 255.0f;
	reg = std::regex("<Projection FOV=(.*?) zNear=(.*?) zFar=(.*?)/>");
	std::regex_search(fData, match, reg);
	Camera::getInstance()->Zoom = std::stof(match.str(1));
	Camera::getInstance()->zNear = std::stof(match.str(2));
	Camera::getInstance()->zFar = std::stof(match.str(3));
	
	reg = std::regex("<PlaneConfig width=(.*?) depth=(.*?) maxHeight=(.*?)/>");
	std::regex_search(fData, match, reg);
	ebx_width->setText(match.str(1));
	ebx_depth->setText(match.str(2));
	ebx_maxHeight->setText(match.str(3));
	reg = std::regex("<PlaneDiff path=\"(.*?)\"/>");
	std::regex_search(fData, match, reg);
	Plane::diffPath = match.str(1);
	reg = std::regex("<PlaneSpec path=\"(.*?)\"/>");
	std::regex_search(fData, match, reg);
	Plane::specPath = match.str(1);
	reg = std::regex("<PlaneNorm path=\"(.*?)\"/>");
	std::regex_search(fData, match, reg);
	Plane::normPath = match.str(1);

	 reg = std::regex("<DirLight x=(.*?) y=(.*?) z=(.*?) ambR=(.*?) ambG=(.*?) ambB=(.*?) diffR=(.*?) diffG=(.*?) diffB=(.*?) specR=(.*?) specG=(.*?) specB=(.*?)/>");
	std::regex_search(fData, match, reg);
	dirLight.direction = glm::normalize(glm::vec3(
		std::stof(match.str(1)),
		std::stof(match.str(2)),
		std::stof(match.str(3))));
	dirLight.ambient =glm::vec3(
		std::stof(match.str(4))/255.0f
		, std::stof(match.str(5)) / 255.0f
		, std::stof(match.str(6)) / 255.0f);

	dirLight.diffuse = glm::vec3(
		std::stof(match.str(7)) / 255.0f
		, std::stof(match.str(8)) / 255.0f
		, std::stof(match.str(9)) / 255.0f);
	dirLight.specular = glm::vec3(
		std::stof(match.str(10)) / 255.0f
		, std::stof(match.str(11)) / 255.0f
		, std::stof(match.str(12)) / 255.0f);
	
	reg = std::regex("<PointLight const=(.*?) linear=(.*?) quadratic=(.*?) ambR=(.*?) ambG=(.*?) ambB=(.*?) diffR=(.*?) diffG=(.*?) diffB=(.*?) specR=(.*?) specG=(.*?) specB=(.*?)/>");
	std::regex_search(fData, match, reg);
	defaultPlightsValues = glm::vec3(
		std::stof(match.str(1)),
		std::stof(match.str(2)),
		std::stof(match.str(3)));
	defaultAmbientColor = glm::vec3(
		std::stof(match.str(4)) 
		, std::stof(match.str(5))
		, std::stof(match.str(6)));

	defaultDiffuseColor = glm::vec3(
		std::stof(match.str(7)) 
		, std::stof(match.str(8))
		, std::stof(match.str(9)));
	defaultSpecColor = glm::vec3(
		std::stof(match.str(10))  
		, std::stof(match.str(11)) 
		, std::stof(match.str(12)) );
	reg = std::regex("<LoadSkybox xPostive=\"(.*?)\" xNegative=\"(.*?)\" yPostive=\"(.*?)\" yNegative=\"(.*?)\" zPostive=\"(.*?)\" zNegative=\"(.*?)\" >(.*?)</LoadSkybox>");
	if (std::regex_search(fData, match, reg)) {
		std::vector<std::string> Faces;
		std::string path = match.str(7);
		Faces.resize(6);
		for (int i = 0; i < 6; i++) {
			Faces[i] =path+ match.str(1 + i);
		}
		skyboxCubemap = Texture::loadCubemap(Faces);
		skyboxCube = new Box();
	}

	
}

void GameSystem::gameLoop()
{

	resetOnAction(CEGUI::EventArgs());//put initData for Proj&lights
	while (_gameState != GameState::EXIT) {
		//Calculate DeltaTime (60 on VSYNC)
		static unsigned int last_time = 0;
		unsigned int current_time = SDL_GetTicks();
		unsigned int elapsed = current_time - last_time;
		float DeltaTime = (float)elapsed / 1000.0f;
		last_time = current_time;
		
		updateViewPort();
		updateGame(DeltaTime);
		
		renderScene();
		collectInput(DeltaTime);//collects input data and process them
		
	}


}

void GameSystem::collectInput(float DeltaTime)
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		//PASS EVENT TO GUI
		_GUI.onSDLEvent(e);
		switch (e.type) {
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_KEYDOWN:
			_inputKeeper->addPressKey(e.key.keysym.sym);
			break;
		case SDL_KEYUP:
			_inputKeeper->addReleaseKey(e.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			
			_inputKeeper->addPressKey(e.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			_inputKeeper->addReleaseKey(e.button.button);
			break;
		case SDL_MOUSEMOTION:
			_inputKeeper->setMousePosition(e.motion.x, e.motion.y);
			break;

		}

	}
	processInput(DeltaTime);

}

void GameSystem::processInput(float DeltaTime)
{
	bool EditBoxActive = isEditBoxActive();
	if (_inputKeeper->isKeyHeld(SDLK_ESCAPE)) {
		_gameState = GameState::EXIT;
	}

	if (_inputKeeper->isKeyPressed(SDL_BUTTON_RIGHT)) {

		setAllEditBoxNotActive();
	}
	
	if (_inputKeeper->isKeyPressed(SDL_BUTTON_LEFT)) {
		
		_modelSelector->getMouseWorldPos();
		processMousePick(DeltaTime);
	}
	if (_inputKeeper->isKeyHeld(SDL_BUTTON_LEFT) && !EditBoxActive && !isInFrameWindowBounds()) {
		ObjectOnDragging();

	}
	//Cant Move while dragging objects simple solution for now
	else {
		if (_inputKeeper->isKeyHeld(SDL_BUTTON_LEFT)) {
			_inputKeeper->addReleaseKey(SDL_BUTTON_LEFT);
			//release left button if ebox active or in fmw
		}
		//SOME HOTKEYS
		if (_inputKeeper->isKeyHeld(SDLK_LCTRL)) {
			if (_inputKeeper->isKeyPressed(SDLK_q)) {
				rdb_2_move->setSelected(true);
			}
			if (_inputKeeper->isKeyPressed(SDLK_w)) {
				rdb_2_scale->setSelected(true);
			}
			if (_inputKeeper->isKeyPressed(SDLK_e)) {
				rdb_2_rotate->setSelected(true);
			}
			if (_inputKeeper->isKeyPressed(SDLK_z)) {
				static bool enabled = true;
				enabled = !enabled;
				chb_x->setSelected(enabled);
			}
			if (_inputKeeper->isKeyPressed(SDLK_x)) {
				static bool enabled = true;
				enabled = !enabled;
				chb_y->setSelected(enabled);
			}
			if (_inputKeeper->isKeyPressed(SDLK_c)) {
				static bool enabled = true;
				enabled = !enabled;
				chb_z->setSelected(enabled);
			}
			if (_inputKeeper->isKeyPressed(SDLK_d)) {
				static bool enabled = false;
				enabled = !enabled;
				chb_wfmode->setSelected(enabled);
			}
			if (_inputKeeper->isKeyHeld(SDLK_LALT)) {//update whenever user click LCTRL+LALT
				updateModelsOnAction(CEGUI::EventArgs());
			}
			if (_inputKeeper->isKeyHeld(SDLK_DELETE)) {//delete selected model on LCTRL + DELETE
				removeOnAction(CEGUI::EventArgs());
			}
		}else if(!EditBoxActive) {//if keyboard inputs check if no currently active edit boxes
				if (_inputKeeper->isKeyHeld(SDLK_w)) {
					_camera->ProcessKeyboard(Camera_Movement::FORWARD, DeltaTime);
				}
				if (_inputKeeper->isKeyHeld(SDLK_s)) {
					_camera->ProcessKeyboard(Camera_Movement::BACKWARD, DeltaTime);
				}
				if (_inputKeeper->isKeyHeld(SDLK_a)) {
					_camera->ProcessKeyboard(Camera_Movement::LEFT, DeltaTime);
				}
				if (_inputKeeper->isKeyHeld(SDLK_d)) {
					_camera->ProcessKeyboard(Camera_Movement::RIGHT, DeltaTime);
				}

				int x, y;
				SDL_GetMouseState(&x, &y);

				/*
				if (_inputKeeper->isKeyHeld(SDLK_UP)) {
					_camera->ProcessMouseMovement(0.0f, 585.0f*DeltaTime);
				}
				if (_inputKeeper->isKeyHeld(SDLK_DOWN)) {
					_camera->ProcessMouseMovement(0.0f, -585.0f*DeltaTime);
				}
				if (_inputKeeper->isKeyHeld(SDLK_LEFT)) {
					_camera->ProcessMouseMovement(-585.0f*DeltaTime, 0.0f);
				}
				if (_inputKeeper->isKeyHeld(SDLK_RIGHT)) {
					_camera->ProcessMouseMovement(585.0f*DeltaTime, 0.0f);
				}
				*/
				
				if (_inputKeeper->isKeyHeld(SDLK_LSHIFT))
				{
					_camera->ProcessMouseMovement(x, y, DeltaTime);
				}
				else
				{
					_camera->reset();
				}
			}
	}
}

void GameSystem::processMousePick(float DeltaTime)
{
	if ( isInFrameWindowBounds())return;
	//Resort objects based on distance from camera
	//speeds up & helps the user to click nearest object the process of finding the object a bit instead of large collision detection computation
	std::sort(GameObjects.begin(), GameObjects.end(), [](PhysicalObject* po1, PhysicalObject* po2) {
		glm::vec3 cameraPos =Camera::getInstance()->getPosition();
		
		float a = glm::length(po1->getPos() - cameraPos);
		float b = glm::length(po2->getPos() - cameraPos);
		return  a<b;
	});

	setAllEditBoxNotActive();//stop all editboxes

	glm::vec3 mouseWorldDir = _modelSelector->getMouseWorldDirection();
	_selectedObject = nullptr;//if i didnt pick a object nullptr the selected object(Deselect)
	Ray* _RAY = new Ray(Camera::getInstance()->getPosition(), mouseWorldDir);

	for (PhysicalObject* p : GameObjects) {
	
		if (dynamic_cast<Sphere*>(p)) {
			if (static_cast<SphereCollider*>(p->getCollider())->isIntersecting(_RAY)) {
						_selectedObject = p;
						break;
			}
		}
			else {//100% model OR BOX(AABB COllider )
				if (static_cast<AABBCollider*>(p->getCollider())->isIntersecting(_RAY)) {							
					_selectedObject = p;
					if (dynamic_cast<LightLamp*>(_selectedObject)) {
						rdb_2_move->setSelected(true);
					}
					break;
				}
			}
		
	}
	delete _RAY;

	getModelDetails();//Display model details on model stats tab
}

void GameSystem::updateViewPort()
{
	int w, h;
	SDL_GetWindowSize(_window,&w,&h);
	glViewport(0, 0, w, h);
	//TODO FIX THIS  POOR DESIGN
	Camera::Window_Width = w;
	Camera::Window_Height = h;
	Shader::SCR_WIDTH = w;
	Shader::SCR_HEIGHT = h;
	Shader::nearSpace = Camera::getInstance()->zNear;
	Shader::nearSpace = Camera::getInstance()->zFar;

}

void GameSystem::updateGame(float DeltaTime)
{

	_inputKeeper->updateKeys();
	_GUI.update(DeltaTime);
	Camera::getInstance()->update();
	Shader::deltaTime += DeltaTime;
	for (PhysicalObject* p : GameObjects) {
		p->update(DeltaTime);
	}

	
	

}

void GameSystem::renderGame()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (chb_wfmode->isSelected()) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (scenePlane != nullptr&&!chb_hidePlane->isSelected()) {
		scenePlane->render();
	}
	
	for (PhysicalObject* p : GameObjects) {
		p->draw();
	}

	if (_selectedObject != nullptr) {
		renderSelectedObject();
		//draw kind of mask around selected object instad of using stencil buffer
	}

	//RENDER SKYBOX LAST
	//check if the box !=nullptr or the textureID==0
	if (skyboxCube != nullptr) {
		renderSkybox();
	}

}

void GameSystem::renderGameSimplified(Shader* sh)
{


	for (PhysicalObject* p : GameObjects) {
		glm::mat4 model;
		p->prepareModelMat(model);
		sh->setMat4("model", model);
		p->drawSimplified();
	}
	if (scenePlane != nullptr && !chb_hidePlane->isSelected()) {
		scenePlane->renderSimplified();
	}
}

void GameSystem::renderWater()
{
	Camera::getInstance()->setPitch(-Camera::getInstance()->getPitch());
	glm::vec3 camPOS = Camera::getInstance()->getPosition();
	//setting water at y=0;
	Camera::getInstance()->setPosition(glm::vec3(camPOS.x,-camPOS.y,camPOS.z));
	Camera::getInstance()->updateCameraVectors();
	Camera::getInstance()->update();
	sceneWater->BindFBO_Reflection();
	glEnable(GL_CLIP_DISTANCE0);
	renderGame();
	glDisable(GL_CLIP_DISTANCE0);
	Camera::getInstance()->setPitch(-Camera::getInstance()->getPitch());
	Camera::getInstance()->setPosition(camPOS);
	Camera::getInstance()->updateCameraVectors();
	Camera::getInstance()->update();
	sceneWater->BindFBO_Refraction();
	glEnable(GL_CLIP_DISTANCE1);
	renderGame();
	glDisable(GL_CLIP_DISTANCE1);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0, 0, Camera::Window_Width, Camera::Window_Height);

	sceneWater->render();
}

void GameSystem::renderGUI()
{
	//RENDER GUI PROPERLY (WITHOUT DEPTH TEST)
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);//FOR GUI
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	_GUI.draw();
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);

}

void GameSystem::renderShadowMap()
{
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f,Camera::getInstance()->zNear, Camera::getInstance()->zFar);
	glm::mat4 lightView = glm::lookAt(glm::vec3(-40.0f, 80.0f, -20.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	if (scenePlane != nullptr) {
		//I RENDER SHADOW TO PLANE ONLY SO USE PLANE SHADER AND USE SAME LIGHT SPACE MATRIX ONLY
		scenePlane->getShader()->use();
		scenePlane->getShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		Shader::Stop();
	}
	//PROCEED WITH SHADOW SHADER NORMALLY
	Shader::shadowSh->use();
	Shader::shadowSh->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glViewport(0, 0, Camera::getInstance()->Window_Width, Camera::getInstance()->Window_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, Shader::depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);//for peter panning problem
	renderGameSimplified(Shader::shadowSh);
	glCullFace(GL_BACK);
	Shader::Stop();
	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GameSystem::renderScene()
{
	
	renderShadowMap();
	renderGame();
	if (sceneWater != nullptr && !chb_hideWater->isSelected())renderWater();
	
	renderGUI();
	SDL_GL_SwapWindow(_window);
}

void GameSystem::renderSkybox()
{
	//should be drawn last
	glCullFace(GL_FRONT);//because  we are inside it
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxSh->use();
	glm::mat4 view = glm::mat4(glm::mat3(Camera::getInstance()->getViewMatrix())); // remove translation from the view matrix
	skyboxSh->setMat4("view", view);
	skyboxSh->setMat4("projection", Camera::getInstance()->getProjMatrix());
	// skybox cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP,skyboxCubemap);
///DRAW BOX SIMPLIFIED
	skyboxCube->drawSimplified();
	glDepthFunc(GL_LESS); // set depth function back to default
	glCullFace(GL_BACK);
}

void GameSystem::renderSelectedObject()
{

	glm::mat4 model;
	model = glm::translate(model, _selectedObject->getPos());
	if (dynamic_cast<Sphere*>(_selectedObject) || dynamic_cast<Model*>(_selectedObject)) {
		//same rotation as sphere for better shape
		model = glm::rotate(model, glm::radians(_selectedObject->getRotModel().x), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-_selectedObject->getRotModel().y), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(_selectedObject->getRotModel().z), glm::vec3(1.0f, 1.0f, 0.0f));

		model = glm::scale(model, glm::vec3(getVecMax(_selectedObject->getScalModel()) + 0.004f));

	}
	else {
		model = glm::scale(model, _selectedObject->getScalModel() + 0.004f);
	}
	//same scale with some offset(i just try to avoid using stencil for now xD)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	_selectedObject->draw(selectedObjectSh,model);
	if (!chb_wfmode->isSelected()) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	Shader::Stop();

}


void GameSystem::ASearch()
{
	/*
		node : i am using the vertices and the indices generate from the shpere
		as the data for the nav mesh
	*/
	Sphere* sphere = new Sphere();

	std::vector<glm::vec3> vertices;
	vertices.reserve(sphere->getVertices().size());

	for (const auto& vertex : sphere->getVertices())
	{
		vertices.emplace_back(vertex.position);
	}

	const std::vector<unsigned int>& indices = sphere->getIndices();

	NavMesh* navMesh = new NavMesh();
	navMesh->Generate(vertices, indices);

	HENode* start = navMesh->GetNodes().front();
	HENode* end = navMesh->GetNodes().back();

	std::vector<HENode*> path = navMesh->FindPath(start, end);
	std::cout << glm::length(start->GetPosition() - end->GetPosition()) << std::endl;

	for (const auto& node : path)
	{
		glm::vec3 pos = node->GetPosition();
		std::cout << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
	}

}

void GameSystem::ObjectOnDragging()
{
	glm::vec3 mouseOffSet = _modelSelector->getMouseWorldPosOffset();
	if (_selectedObject != nullptr) {
		float distFromCamFactor = glm::length(_selectedObject->getPos() - _camera->getPosition());
		if (rdb_2_move->isSelected()) {
			MoveSelectedObject(mouseOffSet,distFromCamFactor);
				
		}else
		if (rdb_2_scale->isSelected()) {
			ScaleSelectedObject(mouseOffSet,distFromCamFactor);
			
		}else
		if (rdb_2_rotate->isSelected()) {
			RotateSelectedObject(mouseOffSet,distFromCamFactor);
			
		}
		displayModelStats();
	}
}

void GameSystem::MoveSelectedObject(glm::vec3 MouseOffset, float distFromCam)
{
	glm::vec3 objectPos = _selectedObject->getPos();
	if (chb_x->isSelected()) {
		objectPos += glm::vec3(MouseOffset.x*distFromCam, 0.0f, 0.0f);
		_selectedObject->setPos(objectPos);
	}
	if (chb_y->isSelected()) {
		objectPos += glm::vec3(0.0f, MouseOffset.y * distFromCam, 0.0f);
		_selectedObject->setPos(objectPos);
	}
	if (chb_z->isSelected()) {
		objectPos += glm::vec3(0.0f, 0.0f, MouseOffset.z * distFromCam);
		_selectedObject->setPos(objectPos);
	}

}

void GameSystem::ScaleSelectedObject(glm::vec3 MouseOffset, float distFromCam)
{
	if (dynamic_cast<LightLamp*>(_selectedObject))return;
	glm::vec3 objectScale= _selectedObject->getScalModel();
	if (chb_x->isSelected()) {
		objectScale += glm::vec3(MouseOffset.x*distFromCam, 0.0f, 0.0f);
		_selectedObject->setScalModel(objectScale);
	}
	if (chb_y->isSelected()) {
		objectScale += glm::vec3(0.0f, MouseOffset.y * distFromCam, 0.0f);
		_selectedObject->setScalModel(objectScale);
	}
	if (chb_z->isSelected()) {
		objectScale += glm::vec3(0.0f, 0.0f, MouseOffset.z * distFromCam);
		_selectedObject->setScalModel(objectScale);
	}
}

void GameSystem::RotateSelectedObject(glm::vec3 MouseOffset, float distFromCam)
{
	if (dynamic_cast<LightLamp*>(_selectedObject))return;
	float toAngleFactor = 90.0f;//as number to rotate is so small for angles so we multiply by some factor
	glm::vec3 objectRotation = _selectedObject->getRotModel();
	if (chb_x->isSelected()) {
		objectRotation += glm::vec3(MouseOffset.x*distFromCam*toAngleFactor, 0.0f, 0.0f);
		_selectedObject->setRotModel(objectRotation);
	}
	if (chb_y->isSelected()) {
		objectRotation += glm::vec3(0.0f, MouseOffset.y * distFromCam*toAngleFactor, 0.0f);
		_selectedObject->setRotModel(objectRotation);
	}
}

void GameSystem::saveGeneralSettings(std::ofstream& oFile)
{
	oFile << "<General SCR_WIDTH="+getString(Camera::getInstance()->Window_Width);
	oFile << " SCR_HEIGHT="+getString( Camera::getInstance()->Window_Height);
	oFile << " FOV=" + getString(Camera::getInstance()->Zoom);
	oFile << " zNear=" + getString(Camera::getInstance()->zNear);
	oFile << " zFar=" + getString(Camera::getInstance()->zFar);
	oFile << " />\n\n";
	if (scenePlane != nullptr) {
		oFile << "<Terrain width=" + getString(scenePlane->getWidth());
		oFile << " depth=" + getString(scenePlane->getDepth());
		oFile << " maxHeight=" + getString(scenePlane->getMaxHeight());
		oFile << " heightMap=" + scenePlane->getHeightMapPath();
		oFile << " />\n\n";
	}



}

void GameSystem::setAllEditBoxNotActive()
{
	ebx_name->deactivate();
	ebx_pos_x->deactivate();
	ebx_pos_y->deactivate();
	ebx_pos_z->deactivate();
	ebx_scal_x->deactivate();
	ebx_scal_y->deactivate();
	ebx_scal_z->deactivate();
	ebx_rot_x->deactivate();
	ebx_rot_y->deactivate();
	ebx_rot_z->deactivate();
	ebx_fileName->deactivate();
	ebx_fov->deactivate();
	ebx_near->deactivate();
	ebx_far->deactivate();
	ebx_constant->deactivate();
	ebx_linear->deactivate();
	ebx_quadratic->deactivate();
	ebx_dir_x->deactivate();
	ebx_dir_y->deactivate();
	ebx_dir_z->deactivate();
	ebx_amb_r->deactivate();
	ebx_amb_g->deactivate();
	ebx_amb_b->deactivate();
	ebx_diff_r->deactivate();
	ebx_diff_g->deactivate();
	ebx_diff_b->deactivate();
	ebx_spec_r->deactivate();
	ebx_spec_g->deactivate();
	ebx_spec_b->deactivate();
	ebx_width->deactivate();
	ebx_depth->deactivate();
	ebx_maxHeight->deactivate();

}

bool GameSystem::isEditBoxActive()
{//This function helps if im typing into edit box so it disable movement :D
	//string edit boxes
	if (ebx_name->isActive())return true;
	if (ebx_fileName->isActive())return true;
	//editor edit boxes
	if (ebx_pos_x->isActive())return true;
	if (ebx_pos_y->isActive())return true;
	if (ebx_pos_z->isActive())return true;
	if (ebx_scal_x->isActive())return true;
	if (ebx_scal_y->isActive())return true;
	if (ebx_scal_z->isActive())return true;
	if (ebx_rot_x->isActive())return true;
	if (ebx_rot_y->isActive())return true;
	if (ebx_rot_z->isActive())return true;
	//proj & lights edit boxes;
	if (ebx_fov->isActive())return true;
	if (ebx_near->isActive())return true;
	if (ebx_far->isActive())return true;
	if (ebx_dir_x->isActive())return true;
	if (ebx_dir_y->isActive())return true;
	if (ebx_dir_z->isActive())return true;
	if (ebx_constant->isActive())return true;
	if (ebx_linear->isActive())return true;
	if (ebx_quadratic->isActive())return true;
	if (ebx_amb_r->isActive())return true;
	if (ebx_amb_g->isActive())return true;
	if (ebx_amb_b->isActive())return true;
	if (ebx_diff_r->isActive())return true;
	if (ebx_diff_g->isActive())return true;
	if (ebx_diff_b->isActive())return true;
	if (ebx_spec_r->isActive())return true;
	if (ebx_spec_g->isActive())return true;
	if (ebx_spec_b->isActive())return true;
	//terrain settings
	if (ebx_width->isActive())return true;
	if (ebx_depth->isActive())return true;
	if (ebx_maxHeight->isActive())return true;
	

	return false;
}

bool GameSystem::isInFrameWindowBounds()
{
	float x= _inputKeeper->getMousePosition().x;
	float y= _inputKeeper->getMousePosition().y;
	if(fmw_ModelStats->isHit(CEGUI::Vector2f(x, y), true))return true;
	if(fmw_ModelEditor->isHit(CEGUI::Vector2f(x, y), true))return true;
	if(fmw_SaveLoad->isHit(CEGUI::Vector2f(x, y), true))return true;
	if (fmw_projLights->isHit(CEGUI::Vector2f(x, y), true))return true;
	if (fmw_TerrainSettings->isHit(CEGUI::Vector2f(x, y), true))return true;
	return false;
}


void GameSystem::getModelDetails()
{
	resetOnAction(CEGUI::EventArgs());
	if (_selectedObject == nullptr) {
		hideModelStats();
	}
	else {
		displayModelStats();
	}
}

void GameSystem::displayModelStats()
{//getString defined in ->Utility.cpp gives us float with certain precision
	ebx_name->setText(_selectedObject->getName());
	lab_type->setText("Type: "+ _selectedObject->getType());
	ebx_pos_x->setText(getString(_selectedObject->getPos().x));
	ebx_pos_y->setText(getString(_selectedObject->getPos().y));
	ebx_pos_z->setText(getString(_selectedObject->getPos().z));
	ebx_scal_x->setText(getString(_selectedObject->getScalModel().x));
	ebx_scal_y->setText(getString(_selectedObject->getScalModel().y));
	ebx_scal_z->setText(getString(_selectedObject->getScalModel().z));
	ebx_rot_x->setText(getString(_selectedObject->getRotModel().x));
	ebx_rot_y->setText(getString(_selectedObject->getRotModel().y));
	ebx_rot_z->setText(getString(_selectedObject->getRotModel().z));
}

void GameSystem::hideModelStats()
{
	ebx_name->setText("");
	lab_type->setText("Type: ");
	ebx_pos_x->setText("");
	ebx_pos_y->setText("");
	ebx_pos_z->setText("");
	ebx_scal_x->setText("");
	ebx_scal_y->setText("");
	ebx_scal_z->setText("");
	ebx_rot_x->setText("");
	ebx_rot_y->setText("");
	ebx_rot_z->setText("");

}

bool GameSystem::updateModelsOnAction(const CEGUI::EventArgs & e)
{
	if (_selectedObject == nullptr)return false;
	_selectedObject->setName(ebx_name->getText().c_str());
	float x, y, z;
	x= std::stof(ebx_pos_x->getText().c_str());
	y= std::stof(ebx_pos_y->getText().c_str());
	z= std::stof(ebx_pos_z->getText().c_str());
	_selectedObject->setPos(glm::vec3(x,y,z));
	if (dynamic_cast<LightLamp*>(_selectedObject)) {
		displayModelStats();
		return false;
	}
	x = std::stof(ebx_scal_x->getText().c_str());
	y = std::stof(ebx_scal_y->getText().c_str());
	z = std::stof(ebx_scal_z->getText().c_str());
	_selectedObject->setScalModel(glm::vec3(x, y, z));
	x = std::stof(ebx_rot_x->getText().c_str());
	y = std::stof(ebx_rot_y->getText().c_str());
	z = std::stof(ebx_rot_z->getText().c_str());
	_selectedObject->setRotModel(glm::vec3(x, y, z));
	return true;
}

bool GameSystem::removeOnAction(const CEGUI::EventArgs & e)
{
	if (_selectedObject == nullptr)return false;
	for (std::vector<PhysicalObject*>::iterator it = GameObjects.begin(); it !=GameObjects.end(); it++) {
		if ((*it)->getName()==_selectedObject->getName()) {
			if (dynamic_cast<LightLamp*>(*it)){
				PointLight* p = static_cast<LightLamp*>(*it)->getPointLight();
				removeLight(p);
			}
			(*it)->destroy();
			GameObjects.erase(it);
			delete _selectedObject;
			_selectedObject = nullptr;
			getModelDetails();
			return true;
		}
	}

		
	
	return false;
}
void GameSystem::removeLight(PointLight* p) {
	for (std::vector<PointLight*>::iterator it = pLights.begin(); it != pLights.end(); it++) {
		if ((*it)->name==p->name) {
			pLights.erase(it);
			delete p;
			
			getModelDetails();
			return;
		}
	}
}
bool GameSystem::resetOnAction(const CEGUI::EventArgs & e)
{
	//set Projection & Lights initial values
	ebx_fov->setText(getString(_camera->Zoom));
	ebx_near->setText(getString(_camera->zNear));
	ebx_far->setText(getString(_camera->zFar));
	ebx_dir_x->setText(getString(dirLight.direction.x));
	ebx_dir_y->setText(getString(dirLight.direction.y));
	ebx_dir_z->setText(getString(dirLight.direction.z));
	ebx_constant->setText(getString(defaultPlightsValues.x));
	ebx_linear->setText(getString(defaultPlightsValues.y));
	ebx_quadratic->setText(getString(defaultPlightsValues.z));
	ebx_amb_r->setText(getString(defaultAmbientColor.x));
	ebx_amb_g->setText(getString(defaultAmbientColor.y));
	ebx_amb_b->setText(getString(defaultAmbientColor.z));
	ebx_diff_r->setText(getString(defaultDiffuseColor.x));
	ebx_diff_g->setText(getString(defaultDiffuseColor.y));
	ebx_diff_b->setText(getString(defaultDiffuseColor.z));
	ebx_spec_r->setText(getString(defaultSpecColor.x));
	ebx_spec_g->setText(getString(defaultSpecColor.y));
	ebx_spec_b->setText(getString(defaultSpecColor.z));

	if (_selectedObject == nullptr)return false;
	displayModelStats();

	return true;
}
std::string GameSystem::ShaderToUse() {
	CEGUI::ListboxTextItem* mItem = static_cast<CEGUI::ListboxTextItem*>(cmb_models->getSelectedItem());
	CEGUI::ListboxTextItem* sItem = static_cast<CEGUI::ListboxTextItem*>(cmb_shaders->getSelectedItem());
	std::string shToUse;
	if (sItem == nullptr || sItem->getText() == "Default") {
		if (mItem->getText() == "Sphere") {
			shToUse = "Default/Sphere/Sphere";
		}
		else if (mItem->getText() == "Box") {
			shToUse = "Default/Box/Box";
		}
		else {
			shToUse = "Default/Model/Model";
		}
	}else if(sItem->getText() == "Default Norm. Mapping"){
		//NORM SHADERS ARE SHADERS IMPLEMENTED USING TANGENT SPACE SO IT MAKES A BIT BETTER LIGHTING WITH ROTATION
		//AND ALLOWS NORMAL MAPPING
		
		//CONS:
		//I USED THE FRAGMENT METHOD BECAUSE IT WAS EASIER FOR ME ITS BAD FOR PERFORMANCE USE YOUR OWN SHADERS
		if (mItem->getText() == "Sphere") {
			shToUse = "Default/Sphere/normSphere";
		}
		else if (mItem->getText() == "Box") {
			shToUse = "Default/Box/normBox";
		}
		else {
			shToUse = "Default/Model/normModel";
		}
	}else {
		shToUse = sItem->getText().c_str();
	}
	return shToUse;
}

TexturePathSTRUCT GameSystem::TexToUse() {
	CEGUI::ListboxTextItem* TexItem = static_cast<CEGUI::ListboxTextItem*>(cmb_textures->getSelectedItem());
	TexturePathSTRUCT tp;
	std::string TexToUse;
	if (TexItem == nullptr) {
		tp = comboBox_Textures.find("COLOR_WHITE.png")->second;
	}
	else {
		tp = comboBox_Textures.find(TexItem->getText().c_str())->second;
		//if user used default keyword we use the default spec and norm colors 
		if (tp.normFile == "default")tp.normFile = "res/Textures/Default/NO_NORMALMAP.png";
		if (tp.specFile == "default")tp.specFile = "res/Textures/Default/NO_SPECMAP.png";
	}
	return tp;
}

bool GameSystem::placeModelsOnAction(const CEGUI::EventArgs & e)
{
	
	CEGUI::ListboxTextItem* modelItem=static_cast<CEGUI::ListboxTextItem*>(cmb_models->getSelectedItem());
	if (modelItem == nullptr) {
		std::cout << "No Model was Selected" << std::endl;
		return false;
	}
	
	std::string selectedModel= modelItem->getText().c_str();
	TexturePathSTRUCT tp = TexToUse();
	if (selectedModel=="Sphere")GameObjects.push_back(new Sphere(1.0f,30, 30, ShaderToUse(), chb_geoShader->isSelected(),tp.diffFile,tp.normFile,tp.specFile ));
	else if (selectedModel == "Box")GameObjects.push_back(new Box(1.0f, 1.0f, 1.0f, ShaderToUse(), chb_geoShader->isSelected(),tp.diffFile,tp.normFile,tp.specFile));
	else {
		std::string modelPath = comboBox_Models.find(selectedModel)->second;
		GameObjects.push_back(new Model(modelPath + selectedModel, modelPath, selectedModel, ShaderToUse(), chb_geoShader->isSelected()));
		GameObjects.back()->setType(selectedModel);
	}
	return true;
}

bool GameSystem::saveOnAction(const CEGUI::EventArgs & e)
{
	
	std::ofstream oFile(SaveLoadPath+ebx_fileName->getText().c_str());
	saveGeneralSettings(oFile);
	for (PhysicalObject* p : GameObjects) {
		oFile << p->getInfo();
	}
	oFile.close();

	return true;
}

bool GameSystem::loadOnAction(const CEGUI::EventArgs & e)
{
	newOnAction(CEGUI::EventArgs());//clear current session
	TerrainInfo terrainInfo;
	IOManager::getInstance()->loadObjects(GameObjects, SaveLoadPath + ebx_fileName->getText().c_str(), comboBox_Models,terrainInfo);
	if (terrainInfo.dimensions.x != -1) {//there were a terrain 
		float width= terrainInfo.dimensions.x, depth= terrainInfo.dimensions.y, maxHeight= terrainInfo.dimensions.z;
		scenePlane = new Plane("Default/Plane/Plane", maxHeight, width, depth, width / 2.0f, depth / 2.0f,terrainInfo.filePath);
		sceneWater = new Water("Default/Water/Water", width, depth);
	}
	return true;
}

bool GameSystem::newOnAction(const CEGUI::EventArgs & e)
{
	_selectedObject = nullptr;
	for (PhysicalObject* p : GameObjects) {
		if (dynamic_cast<LightLamp*>(p)) {
			PointLight* pL = static_cast<LightLamp*>(p)->getPointLight();
			removeLight(pL);
		}
		p->destroy();
		delete p;
	}
	GameObjects.clear();
	if (scenePlane != nullptr) {
		scenePlane->destroy();
		delete scenePlane;
		scenePlane = nullptr;
	}
	if (sceneWater != nullptr) {
		delete sceneWater;
		sceneWater = nullptr;
	}
	//reset number of instants of each type 
	Box::numOfBoxes = 0;
	Sphere::numOfSpheres = 0;
	Model::numOfModels = 0;
	LightLamp::numOfLamps = 0;

	return true;
}

bool GameSystem::updatePLOnAction(const CEGUI::EventArgs & e)
{
	_camera->Zoom= std::stof(ebx_fov->getText().c_str());
	_camera->zNear= std::stof(ebx_near->getText().c_str());
	_camera->zFar= std::stof(ebx_far->getText().c_str());
	dirLight.direction.x = std::stof(ebx_dir_x->getText().c_str());
	dirLight.direction.y = std::stof(ebx_dir_y->getText().c_str());
	dirLight.direction.z = std::stof(ebx_dir_z->getText().c_str());
	dirLight.direction = glm::normalize(dirLight.direction);
	return true;
}

bool GameSystem::placeLightOnAction(const CEGUI::EventArgs & e)
{
	PointLight* p = new PointLight();
	p->ambient.r = std::stof(ebx_amb_r->getText().c_str()) / 255.0f;
	p->ambient.g = std::stof(ebx_amb_g->getText().c_str()) / 255.0f;
	p->ambient.b = std::stof(ebx_amb_b->getText().c_str()) / 255.0f;
	p->diffuse.r = std::stof(ebx_diff_r->getText().c_str())/255.0f;
	p->diffuse.g = std::stof(ebx_diff_g->getText().c_str()) / 255.0f;
	p->diffuse.b = std::stof(ebx_diff_b->getText().c_str()) / 255.0f;
	p->specular.r = std::stof(ebx_spec_r->getText().c_str()) / 255.0f;
	p->specular.g = std::stof(ebx_spec_g->getText().c_str()) / 255.0f;
	p->specular.b = std::stof(ebx_spec_b->getText().c_str()) / 255.0f;
	p->constant= std::stof(ebx_constant->getText().c_str());
	p->linear = std::stof(ebx_linear->getText().c_str());
	p->quadratic = std::stof(ebx_quadratic->getText().c_str());
	pLights.push_back(p);
	LightLamp* LL=new LightLamp(p);
	p->name = LL->getName();
	GameObjects.push_back(LL);
	return true;
}

std::string GameSystem::heightMapToUse()
{
	CEGUI::ListboxTextItem* hMapItem = static_cast<CEGUI::ListboxTextItem*>(cmb_heightMaps->getSelectedItem());

	return comboBox_hMaps.find(hMapItem->getText().c_str())->second;
}

bool GameSystem::genPlaneOnAction(const CEGUI::EventArgs & e)
{
	CEGUI::ListboxTextItem* hMapItem = static_cast<CEGUI::ListboxTextItem*>(cmb_heightMaps->getSelectedItem());
	if (hMapItem == nullptr)return false;
	float width = std::stof(ebx_width->getText().c_str());
	float depth = std::stof(ebx_depth->getText().c_str());
	float maxHeight = std::stof(ebx_maxHeight->getText().c_str());
	if (width < 4.0f || depth < 4.0f || maxHeight < 0.0f)return false;
	if (scenePlane != nullptr)delete scenePlane;
	if (sceneWater != nullptr)delete sceneWater;
	scenePlane = new Plane("Default/Plane/Plane", maxHeight, width, depth, width / 2.0f, depth / 2.0f,heightMapToUse());
	sceneWater = new Water("Default/Water/Water", width, depth);
	return true;
}

bool GameSystem::genRandomPlaneOnAction(const CEGUI::EventArgs & e)
{
	float width = std::stof(ebx_width->getText().c_str());
	float depth = std::stof(ebx_depth->getText().c_str());
	float maxHeight = std::stof(ebx_maxHeight->getText().c_str());
	if (width < 4.0f || depth < 4.0f || maxHeight < 0.0f)return false;
	if (scenePlane != nullptr)delete scenePlane;
	if (sceneWater != nullptr)delete sceneWater;
	scenePlane = new Plane("Default/Plane/Plane", maxHeight,width,depth,(int)(width/2.0f), (int)(depth/2.0f));
	sceneWater = new Water("Default/Water/Water",width,depth);
	IOManager::getInstance()->saveHeightMapImage(SaveLoadPath+"RandomGenerator_H_Map.bmp",scenePlane->getRGen()->getHMap(), width, depth, (int)(width / 2.0f), (int)(depth / 2.0f), scenePlane->getRGen()->getActualMaxHeight());
	scenePlane->setHeightMapPath(SaveLoadPath + "RandomGenerator_H_Map.bmp");
	//BECAUSE HEIGHMAP LOADING DOESNT MATCH RANDOM GENERATION SO I LOAD THE RESULT HEIGHTMAP 
	//SO IF USE SAVES HIS WORK AFTER LOADING HE WILL FIND IT THE SAME WAY IT WAS SAVED
	scenePlane= new Plane("Default/Plane/Plane", maxHeight, width, depth, width / 2.0f, depth / 2.0f,"res/Levels/RandomGenerator_H_Map.bmp" );
	return true;
}

