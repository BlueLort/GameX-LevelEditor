#include "RandomGenerator.h"

const float ROUGHNESS = 0.25f;
const int OCTAVES =5;

RandomGenerator::RandomGenerator(float mHeight) :maxHeight(mHeight), rd{}, mt{ rd() }, dist{ -1.0f, 1.0f }
{
	seed = 10000*(1.0f-dist(mt));
}

RandomGenerator::RandomGenerator()
{
}

void RandomGenerator::setOffsets(float offx, float offz)
{
	offsetX = offx;
	offsetZ = offz;
}

float RandomGenerator::getHeight(float x, float z)
{

	
	std::map<std::pair<float, float>, float>::iterator ite= xyValues.find(std::pair<float, float>(x, z));
	if ( ite!= xyValues.end()) {
		return ite->second;
		}
	float finalHeight = 0;
	float factor = (float)pow(2, OCTAVES - 1);
	for (int i = 0; i < OCTAVES; i++) {
		float freq = (float)(pow(2, i) / factor);
		float amp = (float)(pow(ROUGHNESS, i)*maxHeight);
		finalHeight += getInterpolatedHeight(x*freq, z *freq)*amp;
	}
	if (actualMaxHeight < fabs(finalHeight)*2.0f) {
		actualMaxHeight = fabs(finalHeight)*2.0f;
	}
	xyValues.insert(std::pair<std::pair<float,float>,float>(std::pair<float,float>(x,z) ,finalHeight));
	return finalHeight;
}


float RandomGenerator::getNoise(float x, float z)
{
	mt.seed((x*345) + (z*432)  + seed);
	return dist(mt);
}

float RandomGenerator::getSmoothNoise(float x, float z)
{/**
         c s c
		 s ? s
		 c s c
 */

	float corners = (getNoise(x + offsetX, z + offsetZ) + getNoise(x - offsetX, z + offsetZ) +
		getNoise(x + offsetX, z - offsetZ) + getNoise(x - offsetX, z - offsetZ)) / 16.0f;
	float sides= (getNoise(x + offsetX, z ) + getNoise(x - offsetX, z) + getNoise(x , z - offsetZ) + getNoise(x , z + offsetZ)) /8.0f;
	float center = getNoise(x, z)/4.0f;
	return corners+sides+center;
}

float RandomGenerator::getInterpolatedHeight(float x, float z)
{
	///  ver1---->interpolation1<----- ver2
	///          float place 
	///        
	///  ver3---->interpolation2<-----ver4
	///and then interpolation in Z axis
	int intX = (int)x;
	int intZ = (int)z;
	float floatX = x - intX;
	float floatZ = z - intZ;
	float vert1 = getSmoothNoise(intX, intZ);
	float vert2 = getSmoothNoise(intX+ offsetX, intZ);
	float vert3 = getSmoothNoise(intX, intZ+ offsetZ);
	float vert4 = getSmoothNoise(intX+ offsetX, intZ+ offsetZ);
	float interpolation1 = interpolate(vert1, vert2, floatX);
	float interpolation2 = interpolate(vert3, vert4, floatX);
	return interpolate(interpolation1, interpolation2, floatZ);
}

RandomGenerator::~RandomGenerator()
{
	xyValues.clear();
}
