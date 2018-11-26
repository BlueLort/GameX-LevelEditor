#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "SphereCollider.h"
#include "HEVertex.h"

class HalfEdge;

class HENode
{
private:
	glm::vec3        	m_Position;
	HENode             *m_Parent;
	HalfEdge       	   *m_Edge;
	bool	        	m_IsWalkable;
	float	        	m_Hcost;
	float		    	m_Gcost;
	unsigned long long  m_QueryID;
	SphereCollider     *m_Collider;
	static unsigned long long  s_CurrentQueryID;

public:
	HENode();
	~HENode();
	std::vector<HENode*> GetWalkableNeighbors();
	std::vector<HEVertex*> GetVertices();

	void SetPosition(const glm::vec3& position);
	void SetParent(HENode* parent);
	void SetHalfEdge(HalfEdge* edge);
	void SetIsWalkable(const bool& walkable);
	void SetHCost(int hcost);
	void SetGCost(int gcost);
	void SetCollider(SphereCollider* collider);
	inline void SetProcessed() { m_QueryID = s_CurrentQueryID; }

	inline const glm::vec3& GetPosition()  const { return m_Position;        }
	inline HENode*			GetParent()	   const { return m_Parent;			 }
	inline HalfEdge*        GetHalfEdge()  const { return m_Edge;            }
	inline const bool&		IsWalkable()   const { return m_IsWalkable;      }
	inline const float&		GetHCost()	   const { return m_Hcost;           }
	inline const float&		GetGCost()     const { return m_Gcost;           }
	inline const float		GetFCost()     const { return m_Hcost + m_Gcost; }
	inline SphereCollider*  GetCollider()  const { return m_Collider;        }

	inline bool IsProcessed() const { return m_QueryID == s_CurrentQueryID; }
	inline static void AdvanceQuery() { ++s_CurrentQueryID; }
};