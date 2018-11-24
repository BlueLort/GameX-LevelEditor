#pragma once
#include "Collider.h"
#include "AABBCollider.h"
class AABBCollider;
class SphereCollider :public Collider
{
public:

	SphereCollider(glm::vec3 Position,float Radius);
	~SphereCollider();

	float getRadius();

	bool isIntersecting(SphereCollider* otherCollider) ;
	bool isIntersecting(AABBCollider* otherCollider) ;
	bool isIntersecting(Ray* _ray);
private:
	
	float _radius;
};

