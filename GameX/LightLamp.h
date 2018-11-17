#pragma once
#include "Box.h"
class LightLamp :
	public Box
{
public:
	LightLamp(PointLight* pLight);
	~LightLamp();
	void draw() override;
	void draw(Shader * sh, glm::mat4 model) override;
	void drawSimplified() override;
	void update(float DeltaTime) override;
	PointLight* getPointLight() { return pointLight; }
	std::string getInfo()override;
	static int numOfLamps;
private:
	PointLight* pointLight;

};

