#include "AABBCollider.h"



AABBCollider::AABBCollider(glm::vec3 Position , glm::vec3 BoxData)
{
	_pos = Position;
	_boxData = BoxData;
	_width = BoxData.x;
	_height = BoxData.y;
	_depth = BoxData.z;
	refreshExtents();
}


AABBCollider::~AABBCollider()
{
}

void AABBCollider::setPosition(glm::vec3 newPos)
{
	_pos = newPos;
	
	
	refreshExtents();
}

void AABBCollider::setScale(glm::vec3 newScale)
{
	scale = newScale;
	_boxData=glm::vec3(_width*scale.x, _height*scale.y, _depth*scale.z);
	refreshExtents();
}

glm::vec3 AABBCollider::getMinExtent()
{
	return _minExtent;
}

glm::vec3 AABBCollider::getMaxExtent()
{
	return _maxExtent;
}

void AABBCollider::setBoxData(glm::vec3 BoxData)
{
	_boxData = BoxData;
	refreshExtents();
}

void AABBCollider::refreshExtents()
{
	_minExtent.x = _pos.x - _boxData.x / 2;
	_minExtent.y = _pos.y - _boxData.y / 2;
	_minExtent.z = _pos.z - _boxData.z / 2;
	_maxExtent.x = _pos.x + _boxData.x / 2;
	_maxExtent.y = _pos.y + _boxData.y / 2;
	_maxExtent.z = _pos.z + _boxData.z / 2;
}

bool AABBCollider::isIntersecting(SphereCollider* otherCollider)
{
	glm::vec3 SphC = otherCollider->getPos();
	float dist_squared = powf(otherCollider->getRadius(), 2.0f); 
	if (SphC.x <_minExtent.x) dist_squared -= powf(SphC.x - _minExtent.x,2.0f);
	else if (SphC.x > _maxExtent.x) dist_squared -= powf(SphC.x - _maxExtent.x,2.0f);
	if (SphC.y < _minExtent.y) dist_squared -= powf(SphC.y- _minExtent.y,2.0f);
	else if (SphC.y >_maxExtent.y) dist_squared -= powf(SphC.y- _maxExtent.y,2.0f);
	if (SphC.z <_minExtent.z) dist_squared -= powf(SphC.z- _minExtent.z,2.0f);
	else if (SphC.z >_maxExtent.z) dist_squared -= powf(SphC.z- _maxExtent.z,2.0f);
	return dist_squared >= 0;
}

bool AABBCollider::isIntersecting(AABBCollider* otherCollider)
{

	glm::vec3 diffDis1 =  _minExtent-otherCollider->getMaxExtent()  ;
	glm::vec3 diffDis2 =  otherCollider->getMinExtent()- _maxExtent ;

	glm::vec3 maxDistances = getVecMax(diffDis1, diffDis2);
	float maxDistance = getVecMax(maxDistances);
	return maxDistance<=0.0f;
}

bool AABBCollider::isIntersecting(Ray * _ray)
{
	//idea from here :https://www.siggraph.org//education/materials/HyperGraph/raytrace/rtinter3.htm

	float tnear = (-10000000.0f);//very small number
	float tfar = (10000000.0f);//very large number

	glm::vec3 tA = this->_minExtent-_ray->RO;
	tA /= _ray->RD;
	glm::vec3 tB= this->_maxExtent - _ray->RO;
	tB /= _ray->RD;

	if (_ray->RD.y == 0.0f || _ray->RD.x == 0.0f|| _ray->RD.z == 0.0f) {
		if (_ray->RO.x< _minExtent.x || _ray->RO.x>_maxExtent.x
			|| _ray->RO.y< _minExtent.y || _ray->RO.y>_maxExtent.y
			|| _ray->RO.z< _minExtent.z || _ray->RO.z>_maxExtent.z) {
			return false;
		}
	}
	//make sure tA is near plane tB is far plane!!

	float t1, t2;
	//X PLANE
	t1 = tA.x < tB.x ? tA.x : tB.x;
	t2 = tA.x > tB.x ? tA.x : tB.x;

	if (t1 > tnear)tnear = t1;
	if (t2 < tfar)tfar = t2;
	if (tnear > tfar||tfar<0.0f)return false;
	//Y PLANE
	t1 = tA.y < tB.y ? tA.y : tB.y;
	t2 = tA.y > tB.y ? tA.y : tB.y;
	
	if (t1 > tnear)tnear = t1;
	if (t2 < tfar)tfar = t2;
	if (tnear> tfar || tfar<0.0f)return false;
	//Z PLANE
	t1 = tA.z < tB.z ? tA.z : tB.z;
	t2 = tA.z > tB.z ? tA.z : tB.z;

	if (t1 > tnear)tnear = t1;
	if (t2 < tfar)tfar = t2;
	if (tnear> tfar|| tfar<0.0f)return false;

	//INTERSECTION !!
	return true;
}
