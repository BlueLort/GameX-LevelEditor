#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Utility.h"
#include "Structs.h"


class Collider
{
public:
	
	virtual void setPosition(glm::vec3 newPos);
	glm::vec3 getPos();
	virtual void setScale(glm::vec3 newScale);

	glm::vec3 getScale();
	Collider();
	~Collider();

protected:
	glm::vec3 _pos;
	glm::vec3 scale = glm::vec3(1.0f);

};

