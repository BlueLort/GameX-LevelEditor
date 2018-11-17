#pragma once
#include "InputKeeper.h"
#include "Camera.h"
class ModelSelector
{
public:
	static ModelSelector* getInstance() { return &ModelSelectorInstance; }
	glm::vec3 getMouseWorldDirection();
	glm::vec3 getMouseWorldPos();
	glm::vec3 getMouseWorldPosOffset();//call getMouseWorldPos to update current and last pos
	ModelSelector();
	~ModelSelector();
private:
	
	static ModelSelector ModelSelectorInstance;
	InputKeeper* _inputKeeper;
	//Drag vars
	glm::vec3 _mousePos_last=glm::vec3(0.0f,0.0f,0.0f);//start pos of mouse on pressing
	glm::vec3 _mousePos_current = glm::vec3(0.0f, 0.0f, 0.0f);;//current mouse pos of mouse


};

