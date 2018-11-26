#include "HENode.h"
#include "HalfEdge.h"

unsigned long long HENode::s_CurrentQueryID = 0;

HENode::HENode()
	: m_Position(glm::vec3(0))
	, m_Parent(nullptr)
	, m_Edge(nullptr)
	, m_IsWalkable(true)
	, m_Hcost(0)
	, m_Gcost(0)
	, m_Collider(nullptr)
	, m_QueryID(0)

{}

HENode::~HENode()
{}



void HENode::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

void HENode::SetParent(HENode* parent)
{
	m_Parent = parent;
}

void HENode::SetHalfEdge(HalfEdge* edge)
{
	m_Edge = edge;
}

void HENode::SetIsWalkable(const bool& walkable)
{
	m_IsWalkable = walkable;
}

void HENode::SetHCost(int hcost)
{
	m_Hcost = hcost;
}

void HENode::SetGCost(int gcost)
{
	m_Gcost = gcost;
}

void HENode::SetCollider(SphereCollider* collider)
{
	m_Collider = collider;
}

std::vector<HENode*> HENode::GetWalkableNeighbors()
{
	std::vector<HENode*> neighbors;
	
	HalfEdge* start = GetHalfEdge();
	HalfEdge* current = start;
	
	do
	{
		HalfEdge* opposite = current->GetOpposite();
		
		if (opposite != nullptr && opposite->GetNode()->IsWalkable())
		{
			neighbors.push_back(opposite->GetNode());
		}

		current = current->GetNext();
	} 
	while (current != start);

	return neighbors;
}

std::vector<HEVertex*> HENode::GetVertices()
{
	std::vector<HEVertex*> vertices;
	vertices.reserve(3);

	HalfEdge* start = GetHalfEdge();
	HalfEdge* current = start;

	do
	{
		vertices.emplace_back(current->GetVertex());
		current = current->GetNext();
	}
	while (current != start);

	return vertices;
}