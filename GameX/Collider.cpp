#include "Collider.h"


void Collider::setPosition(glm::vec3 newPos) { _pos = newPos;}
glm::vec3 Collider::getPos() { return _pos; }
void Collider::setScale(glm::vec3 newScale)
{
	scale = newScale;
}
glm::vec3 Collider::getScale()
{
	return scale;
}
Collider::Collider()
{
}


Collider::~Collider()
{
}
