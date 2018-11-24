#include "HalfEdge.h"
#include "HENode.h"
#include "HEVertex.h"

HalfEdge::HalfEdge()
	: m_Vertex(nullptr)
	, m_Node(nullptr)
	, m_Opposite(nullptr)
	, m_Next(nullptr)
{}

HalfEdge::~HalfEdge() {}

void HalfEdge::SetVertex(HEVertex* vertex)
{
	m_Vertex = vertex;
}

void HalfEdge::SetNode(HENode* node)
{
	m_Node = node;
}

void HalfEdge::SetOppsite(HalfEdge* opposite) 
{
	m_Opposite = opposite;
}

void HalfEdge::SetNext(HalfEdge* next)
{
	m_Next = next;
}

void HalfEdge::SetPrevious(HalfEdge* previous)
{
	m_Previous = previous;
}