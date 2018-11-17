#pragma once
#include <iostream>
#include <SDL/SDL.h>
#include <vector>
#include "Structs.h"
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <glm/glm.hpp>
#include "Shader.h"
const unsigned int texSize = 1024;

extern void calculateTangents(Vertex* v1, Vertex* v2, Vertex* v3);
extern float getVecMax(glm::vec3 vec);
extern glm::vec3 getVecMax(glm::vec3 vec1, glm::vec3 vec2);
extern std::string getString(float num);
extern int clamp(int input, int start, int end);
extern int map(int x, int in_min, int in_max, int out_min, int out_max);
extern float lerp(int start, int end, int percent);
extern float interpolate(float a, float b, float factor);
extern void printError(std::string err);