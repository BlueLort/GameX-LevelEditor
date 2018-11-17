#pragma once
#include <random>
#include <map>
#include "Structs.h"
#include "Utility.h"
class RandomGenerator
{

public:
	RandomGenerator(float mHeight);
	RandomGenerator();
	void setOffsets(float offx, float offz);
	std::map<std::pair<float, float>, float> getHMap() { return xyValues; }
	float getActualMaxHeight() { return actualMaxHeight; }
	float getHeight(float x, float z);
	~RandomGenerator();
private:

	std::map<std::pair<float, float>, float> xyValues;
	float actualMaxHeight=0;
	std::random_device rd;
	std::mt19937 mt;
	std::uniform_real_distribution<float> dist;

	float offsetX = 0, offsetZ = 0;
	int seed;
	float maxHeight=5.0f;
	float getNoise(float x, float z);
	float getSmoothNoise(float x, float z);
	float getInterpolatedHeight(float x, float z);
};

