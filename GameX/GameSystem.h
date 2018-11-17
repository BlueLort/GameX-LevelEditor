#pragma once
#include "GUI.h"
#include <map>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Box.h"
#include "Sphere.h"
#include "Model.h"
#include "Texture.h"
#include "Camera.h"
#include "Shader.h"
#include "Collider.h"
#include "Structs.h"
#include "RandomGenerator.h"
#include "Utility.h"
#include "IOManager.h"
#include "InputKeeper.h"
#include "ModelSelector.h"
#include "LightLamp.h"
#include "Plane.h"
#include "Water.h"


enum class GameState{PLAY,EXIT};
class GameSystem
{
public:
	GameSystem();
	~GameSystem();
	void run();
	static GameSystem* getGameSystemInstance() { return gameSystemInstance; }//defined in cpp file }//singleton

	float getDeltaTime() { return deltaTime; }

private:
	static GameSystem* gameSystemInstance;
	float deltaTime;
	glm::vec4 clearColor;
	GLuint _VBO;
	GLuint _VAO;


	//
	SDL_Window * _window;
	GameState _gameState;
	Plane* scenePlane=nullptr;
	Water* sceneWater = nullptr;

	GLuint skyboxCubemap=0;
	Shader* skyboxSh = nullptr;
	Box* skyboxCube = nullptr;



	ModelSelector * _modelSelector;
	InputKeeper * _inputKeeper;
	Camera* _camera;
	std::vector<PhysicalObject*> GameObjects;
	//defaults for point light
	glm::vec3 defaultPlightsValues;
	glm::vec3 defaultAmbientColor;
	glm::vec3 defaultDiffuseColor;
	glm::vec3 defaultSpecColor;
	
	void initSystem();
	void initGUI();
	void initConfig();

	void prepareShadowFBO();
	void readTexts(std::string fData);
	void readHMaps(std::string fData);
	void readModels(std::string fData);
	void readShaders(std::string fData);
	void readDefaultSettings(std::string fData);
	void gameLoop();
	void collectInput(float DeltaTime);
	void processInput(float DeltaTime);
	void processMousePick(float DeltaTime);
	void updateViewPort();
	void updateGame(float DeltaTime);
	
	void renderGame();
	void renderGameSimplified(Shader* sh);
	void renderWater();
	void renderGUI();
	void renderShadowMap();
	void renderScene();
	void renderSkybox();
	void renderSelectedObject();

	
	

	//MOUSE PICKING FUNCTIONS
	//-------------------------
	void ObjectOnDragging();
	void MoveSelectedObject(glm::vec3 MouseOffset, float distFromCam);
	void ScaleSelectedObject(glm::vec3 MouseOffset, float distFromCam);
	void RotateSelectedObject(glm::vec3 MouseOffset, float distFromCam);

	//GUI STUFF
	//--------------------------------------------------------

	std::string SaveLoadPath;

	void saveGeneralSettings(std::ofstream& oFile);

	std::map<std::string, std::string> comboBox_Models;
	std::map<std::string, TexturePathSTRUCT> comboBox_Textures;
	std::map<std::string, std::string> comboBox_hMaps;
	GUI _GUI;
	PhysicalObject* _selectedObject = nullptr;
	Shader* selectedObjectSh = nullptr;
	//Model Stats tab
	//helper bools and functions
	void setAllEditBoxNotActive();
	bool isEditBoxActive();
	bool isInFrameWindowBounds();

	//MODEL STATS
	CEGUI::FrameWindow* fmw_ModelStats;
	CEGUI::Editbox* ebx_name;
	CEGUI::DefaultWindow* lab_type;
	CEGUI::Editbox* ebx_pos_x;
	CEGUI::Editbox* ebx_pos_y;
	CEGUI::Editbox* ebx_pos_z;
	CEGUI::Editbox* ebx_scal_x;
	CEGUI::Editbox* ebx_scal_y;
	CEGUI::Editbox* ebx_scal_z;
	CEGUI::Editbox* ebx_rot_x;
	CEGUI::Editbox* ebx_rot_y;
	CEGUI::Editbox* ebx_rot_z;
	CEGUI::PushButton* but_updateModels;
	CEGUI::PushButton* but_remove;
	CEGUI::PushButton* but_reset;
	CEGUI::ToggleButton* chb_wfmode;

	void getModelDetails();
	void displayModelStats();
	void hideModelStats();
	bool updateModelsOnAction(const CEGUI::EventArgs& e);
	bool removeOnAction(const CEGUI::EventArgs& e);
	void removeLight(PointLight * p);
	bool resetOnAction(const CEGUI::EventArgs& e);


	
	//---------------------------
	//MODEL Editor
	CEGUI::FrameWindow* fmw_ModelEditor;
	CEGUI::RadioButton* rdb_2_move;
	CEGUI::RadioButton* rdb_2_scale;
	CEGUI::RadioButton* rdb_2_rotate;
	CEGUI::ToggleButton* chb_x;
	CEGUI::ToggleButton* chb_y;
	CEGUI::ToggleButton* chb_z;
	CEGUI::ToggleButton* chb_geoShader;
	CEGUI::Combobox*  cmb_models;
	CEGUI::Combobox*  cmb_shaders;
	CEGUI::Combobox*  cmb_textures;
	CEGUI::PushButton* but_placeModels;
	std::string ShaderToUse();
	TexturePathSTRUCT TexToUse();
	bool placeModelsOnAction(const CEGUI::EventArgs& e);

	//---------------------------
	//---------------------------
	//Save Load
	CEGUI::FrameWindow* fmw_SaveLoad;
	CEGUI::Editbox* ebx_fileName;
	CEGUI::PushButton* but_save;
	CEGUI::PushButton* but_load;
	CEGUI::PushButton* but_new;

	bool saveOnAction(const CEGUI::EventArgs& e);
	bool loadOnAction(const CEGUI::EventArgs& e);
	bool newOnAction(const CEGUI::EventArgs& e);
	//---------------------------
	//---------------------------
	//Projection & Light
	CEGUI::FrameWindow* fmw_projLights;
	CEGUI::Editbox* ebx_fov;
	CEGUI::Editbox* ebx_near;
	CEGUI::Editbox* ebx_far;
	CEGUI::Editbox* ebx_dir_x;
	CEGUI::Editbox* ebx_dir_y;
	CEGUI::Editbox* ebx_dir_z;
	CEGUI::PushButton* but_updatePL;
	CEGUI::Editbox* ebx_constant;
	CEGUI::Editbox* ebx_linear;
	CEGUI::Editbox* ebx_quadratic;
	CEGUI::Editbox* ebx_amb_r;
	CEGUI::Editbox* ebx_amb_g;
	CEGUI::Editbox* ebx_amb_b;
	CEGUI::Editbox* ebx_diff_r;
	CEGUI::Editbox* ebx_diff_g;
	CEGUI::Editbox* ebx_diff_b;
	CEGUI::Editbox* ebx_spec_r;
	CEGUI::Editbox* ebx_spec_g;
	CEGUI::Editbox* ebx_spec_b;
	CEGUI::PushButton* but_placeLight;

	bool updatePLOnAction(const CEGUI::EventArgs& e);
	bool placeLightOnAction(const CEGUI::EventArgs& e);

	//---------------------------
	//---------------------------
	//Terrain Settings
	CEGUI::FrameWindow* fmw_TerrainSettings;
	CEGUI::ToggleButton* chb_hideWater;
	CEGUI::ToggleButton* chb_hidePlane;
	CEGUI::Combobox*  cmb_heightMaps;
	CEGUI::PushButton* but_genPlane;
	CEGUI::PushButton* but_genRndPlane;
	CEGUI::Editbox* ebx_width;
	CEGUI::Editbox* ebx_depth;
	CEGUI::Editbox* ebx_maxHeight;

	std::string heightMapToUse();

	bool genPlaneOnAction(const CEGUI::EventArgs& e);
	bool genRandomPlaneOnAction(const CEGUI::EventArgs& e);
	//---------------------------


};

