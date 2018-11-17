#pragma once
#include "Collider.h"
#include "SphereCollider.h"
class SphereCollider;
class AABBCollider :
	public Collider
{
public:
	AABBCollider(glm::vec3 Position, glm::vec3 BoxData);
	~AABBCollider();
	void setPosition(glm::vec3 newPos) override;
	void setScale(glm::vec3 newScale) override;

	glm::vec3   getMinExtent();
	glm::vec3	getMaxExtent();

	void setBoxData(glm::vec3 BoxData);
	void refreshExtents();

	bool isIntersecting(SphereCollider* otherCollider);
	bool isIntersecting(AABBCollider* otherCollider);
	bool isIntersecting(Ray* _ray);
private:
	float _width, _height, _depth;//base width,height,depth
	glm::vec3 _minExtent;
	glm::vec3 _maxExtent;
	glm::vec3 _boxData;//width,height,depth after scale
};

