#include "HEVertex.h"
#include "HalfEdge.h"

HEVertex::HEVertex()
	: m_Position(glm::vec3(0))
	, m_Edge(nullptr)
{}

HEVertex::~HEVertex()
{
}

void HEVertex::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

void HEVertex::SetHalfEdge(HalfEdge* edge)
{
	m_Edge = edge;
}