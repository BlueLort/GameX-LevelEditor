#pragma once

class HEVertex;
class HENode;

class HalfEdge
{
private:
	// the vertex which the half edge is going to
	HEVertex *m_Vertex;
	
	// the node (face) which the half edge belongs to
	HENode   *m_Node;

	// the half edge oppsoite in direction to the current half edge
	HalfEdge *m_Opposite;

	// the next traversable half edge in the winding order
	HalfEdge *m_Next;

	// the previously traversable half edge in the winding order
	HalfEdge *m_Previous;

public:
	HalfEdge();
	~HalfEdge();

	void SetVertex(HEVertex* vertex);
	void SetNode(HENode* node);
	void SetOppsite(HalfEdge* opposite);
	void SetNext(HalfEdge* next);
	void SetPrevious(HalfEdge* previous);

	inline HEVertex* GetVertex()   const { return m_Vertex;   }
	inline HENode*   GetNode()     const { return m_Node;     }
	inline HalfEdge* GetOpposite() const { return m_Opposite; }
	inline HalfEdge* GetNext()     const { return m_Next;     }
	inline HalfEdge* GetPrevious() const { return m_Previous; }
};