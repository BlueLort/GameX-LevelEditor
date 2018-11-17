#include "ModelSelector.h"

ModelSelector ModelSelector::ModelSelectorInstance=ModelSelector();

glm::vec3 ModelSelector::getMouseWorldDirection()
{
	glm::vec2 mousePOS = _inputKeeper->getMousePosition();
	//get mouse coords in NDC 
	float x, y, z;
	x = (2.0f *mousePOS.x) / Camera::Window_Width - 1.0f;
	y= 1.0f - (2.0f * mousePOS.y) / Camera::Window_Height;
	z = -1.0f;//ray going into the screen
	//CLIP SPACE
	glm::vec4 mouseRayNDC = glm::vec4(x, y, z,1.0f);

	glm::vec4 mouseRayView = glm::inverse(Camera::getInstance()->getProjMatrix()) * mouseRayNDC;
	 mouseRayView = glm::vec4(mouseRayView.x, mouseRayView.y,-1.0f,0.0f);//DIRECTION NOT A POINT

	glm::vec4 mouseRayDir=glm::inverse(Camera::getInstance()->getViewMatrix()) * mouseRayView;
	
	return glm::normalize(mouseRayDir);//Normalize and return world dir
}

glm::vec3 ModelSelector::getMouseWorldPos()
{

	glm::vec3 mouseWorldDir = getMouseWorldDirection();
	_mousePos_last = _mousePos_current;
	_mousePos_current =Camera::getInstance()->getPosition() + mouseWorldDir;
	return _mousePos_current;
}

glm::vec3 ModelSelector::getMouseWorldPosOffset()
{
	getMouseWorldPos();//call to update current and last pos
	return _mousePos_current - _mousePos_last;
}

ModelSelector::ModelSelector()
{
	_inputKeeper = InputKeeper::getInstance();
}


ModelSelector::~ModelSelector()
{
}
