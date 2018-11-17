#pragma once
#include <map>
#include <unordered_map>
#include <glm/glm.hpp>
class InputKeeper
{
public:
	InputKeeper();
	~InputKeeper();
	void updateKeys();//update last keys with current keys 

	static InputKeeper* getInstance() { return &InputKeeperInstance; }
	void addPressKey(unsigned int SDL_KEY);
	void addReleaseKey(unsigned int SDL_KEY);

	void setMousePosition(float x, float y);
	glm::vec2 getMouseOffset();
	glm::vec2 getMousePosition();
	bool isKeyPressed(unsigned int SDL_KEY);
	bool isKeyHeld(unsigned int SDL_KEY);
	bool isKeyReleased(unsigned int SDL_KEY);
	
private:
	glm::vec2 mousePos=glm::vec2(0.0f,0.0f);
	glm::vec2 lastMousePos = glm::vec2(0.0f, 0.0f);
	static InputKeeper InputKeeperInstance;

	std::unordered_map<unsigned int, bool> currentKeys;
	std::unordered_map<unsigned int, bool> lastKeys;
	bool checkCurrent(unsigned int SDL_KEY);
	bool checkLast(unsigned int SDL_KEY);

};

