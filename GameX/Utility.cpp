
#include "Utility.h"

float getVecMax(glm::vec3 vec)
{
	float max = vec.x;
	if (max < vec.y)max = vec.y;
	return max<vec.z?vec.z:max;
}

glm::vec3 getVecMax(glm::vec3 vec1, glm::vec3 vec2)
{
	glm::vec3 results;
	results.x = vec1.x > vec2.x ? vec1.x : vec2.x;
	results.y = vec1.y > vec2.y ? vec1.y : vec2.y;
	results.z = vec1.z > vec2.z ? vec1.z : vec2.z;
	return results;
}

std::string getString(float num)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(3) << num;
	return ss.str();
}

void calculateTangents(Vertex* v1, Vertex* v2, Vertex* v3) {
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::vec3 edge1= v1->position - v2->position;
	glm::vec3 edge2= v3->position - v2->position;
	glm::vec2 deltaUV1= v1->texCoords - v2->texCoords;
	glm::vec2 deltaUV2= v3->texCoords - v2->texCoords;
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = glm::normalize(tangent);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent = glm::normalize(bitangent);
	v1->tangentU = tangent; v1->bitangent = bitangent;
	v2->tangentU = tangent; v2->bitangent = bitangent;
	v3->tangentU = tangent; v3->bitangent = bitangent;
}
int clamp(int input, int start, int end) {
	input=input > end ? end : input;
	input = input < start ? start : input;
	return input;
}
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float lerp(int start, int end, int percent)
{
	return (1.0f-percent)*start+percent*end;
}
float interpolate(float a, float b, float factor)
{
	double phi = factor * FULL_PI;
	float newFactor = (1.0f - (cos(phi)))*0.5f; //NUMBER BETWEEEN 0 to 1
	return a * (1.0f - newFactor) + b * newFactor;
}

void printError(std::string err)
{
	std::cout << err << std::endl;
	system("pause");
	SDL_Quit();
	exit(1);
}
