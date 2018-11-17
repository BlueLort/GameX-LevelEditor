#include "InputKeeper.h"

InputKeeper InputKeeper::InputKeeperInstance = InputKeeper();

InputKeeper::InputKeeper()
{
	
}


InputKeeper::~InputKeeper()
{
}

void InputKeeper::updateKeys()
{
	lastMousePos.x = mousePos.x;
	lastMousePos.y = mousePos.y;
	for (std::unordered_map<unsigned int, bool>::iterator ite = currentKeys.begin(); ite != currentKeys.end(); ite++) {
		lastKeys[ite->first] = ite->second;

	}
}

void InputKeeper::addPressKey(unsigned int SDL_KEY)
{
	currentKeys[SDL_KEY] = true;
}

void InputKeeper::addReleaseKey(unsigned int SDL_KEY)
{
	currentKeys[SDL_KEY] = false;
}

//void InputKeeper::setRelativeMousePosition(float x, float y)
//{
//	relativeMousePos.x = x;
//	relativeMousePos.y = y;
//}

void InputKeeper::setMousePosition(float x, float y)
{
	mousePos.x = x;
	mousePos.y = y;
}

 glm::vec2 InputKeeper::getMouseOffset() {
	 return   glm::vec2(mousePos.x - lastMousePos.x, lastMousePos.y - mousePos.y); }

glm::vec2 InputKeeper::getMousePosition()
{
	return mousePos;
}

bool InputKeeper::isKeyHeld(unsigned int SDL_KEY)
{
	return(checkCurrent(SDL_KEY) && checkLast(SDL_KEY));
}

bool InputKeeper::isKeyPressed(unsigned int SDL_KEY)
{
	return(checkCurrent(SDL_KEY) && !checkLast(SDL_KEY));

}

bool InputKeeper::isKeyReleased(unsigned int SDL_KEY)
{
	return(!checkCurrent(SDL_KEY) && checkLast(SDL_KEY));
}

bool InputKeeper::checkCurrent(unsigned int SDL_KEY)
{
	std::unordered_map<unsigned int, bool>::iterator ite = currentKeys.find(SDL_KEY);
	if (ite != currentKeys.end())return ite->second;
	return false;
}

bool InputKeeper::checkLast(unsigned int SDL_KEY)
{
	std::unordered_map<unsigned int, bool>::iterator ite = lastKeys.find(SDL_KEY);
	if (ite != lastKeys.end())return ite->second;
	return false;
}
