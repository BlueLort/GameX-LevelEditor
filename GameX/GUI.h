#pragma once
#include <string>
#include <iostream>
#include <CEGUI/CEGUI.h>
#include <CEGUI\RendererModules\OpenGL\GL3Renderer.h>
#include <glm/glm.hpp>
#include <SDL/SDL_events.h>
#include <SDL/SDL_timer.h>
class GUI
{
public:
	void init(const std::string resDir);
	void destroy();
	void loadScheme(const std::string& schemeFile);
	void loadLayout(const std::string& layoutFile, const std::string& resourceGroup="");
	CEGUI::Window* createWidget(const std::string& scheme,const glm::vec4& destPercentages,const glm::vec4& destPixels,  const std::string& name = "");
	
	
	static void setWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destPercentages, const glm::vec4& destPixels);
	void loadFont(const std::string& fontFile);
	void setFont(const std::string& fontFile);
	
	void setMouseCursor(const std::string& cursorFile);
	void showMouseCursor();
	void hideMouseCursor();


	void update(float deltaTime);
	void draw();

	static CEGUI::OpenGL3Renderer* getRenderer() { return _renderer; };
	const CEGUI::GUIContext* getContext() { return _context; };
	CEGUI::Window* getRootWindow() { return _rootWindow; };
	//ACTIONS
	void onSDLEvent(SDL_Event& evnt);
	

private:

	static CEGUI::OpenGL3Renderer* _renderer ;
	CEGUI::GUIContext* _context = nullptr;
	CEGUI::Window* _rootWindow = nullptr;
};


