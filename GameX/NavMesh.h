#pragma once

#include "HalfEdge.h"
#include "HEVertex.h"
#include "HENode.h"
#include "Structs.h"

#include <vector>
#include <map>

struct Compare
{
	bool operator()(HENode* lhs, HENode* rhs) const
	{
		if (lhs->GetFCost() == rhs->GetFCost())
		{
			return lhs->GetHCost() < rhs->GetHCost();
		}
		return lhs->GetFCost() < rhs->GetFCost();
	}
};

class NavMesh
{
private:
	std::vector<HENode*> m_Nodes;
	Shader*         m_Shader;
	GLuint			m_VAO[5];//3 vaos for unwalkable walkable and path areas 2 for start and end nodes
	std::map< int, HEVertex* > m_Vertices;
	std::map< std::pair< int, int >, HalfEdge*> m_Edges;
	std::vector<HENode*> m_PathNodes;
	std::vector<glm::vec3>						m_renderVertices;
	glm::vec3									m_startEndVertices[6];
	GLuint										m_renderPartitions[2];//start of unwalkable nodes and start of path nodes
	HENode*										m_startNode;
	HENode*										m_endNode;
public:
	NavMesh();
	void uploadWalkAbleUnwalkableAreas();
	void uploadPathArea();
	void uploadStart();
	void uploadEnd();
	~NavMesh();
	void SetStartNode(HENode* n);
	void SetEndNode(HENode* n);
	void Generate(const std::vector<glm::vec3>& vertices, 
		          const std::vector<unsigned int>& indices);
	void render();
	void Reset();
	HENode* GetNodeByRay(Ray* ray);

	float GetDistance(HENode* start, HENode* end);

	std::vector<HENode*> FindPath(HENode* start, HENode* end);

	inline const std::vector<HENode*>& GetNodes() const { return m_Nodes; }
};