#include "SphereCollider.h"
#include <iostream>



SphereCollider::SphereCollider(glm::vec3 Position, float Radius):_radius(Radius)
{
	_pos = Position;
}


SphereCollider::~SphereCollider()
{
}



 float SphereCollider::getRadius() { return _radius; }



bool SphereCollider::isIntersecting(SphereCollider* otherCollider)
{
	glm::vec3 oPos = otherCollider->getPos();
	float oRadius = otherCollider->getRadius()*getVecMax(otherCollider->getScale());

	float distanceRadius = glm::length(oPos - _pos);
	float totalRadius= getVecMax(otherCollider->getScale()) *oRadius + getVecMax(scale)* _radius;
	return totalRadius > distanceRadius;
}

bool SphereCollider::isIntersecting(AABBCollider* otherCollider)
{
	return otherCollider->isIntersecting(this);
}

bool SphereCollider::isIntersecting(Ray* _ray)
{
	//point of intersection (p)=RO+RD*t 
	//as t is time
	//intersection can happen with 2 points or only 1 
	//project ray to sph on ray dir and if distance from center to that point
	//bigger than radius then it didnt hit
	glm::vec3 rayToSph = this->_pos - _ray->RO;
	float t = glm::dot(rayToSph, _ray->RD );
	return glm::length((_ray->RO + (_ray->RD*t))- this->_pos) < this->_radius*getVecMax(scale);


}
