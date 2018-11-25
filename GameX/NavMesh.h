#pragma once

#include "HalfEdge.h"
#include "HEVertex.h"
#include "HENode.h"
#include "Structs.h"

#include <vector>
#include <map>

struct Compare
{
	bool operator()(const HENode* lhs, const HENode* rhs) const
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
	GLuint			m_VAO_WALKABLE;
	GLuint			m_VAO_UNWALKABLE;
	GLuint			m_VAO_PATH;
	std::map< int, HEVertex* > m_Vertices;
	std::map< std::pair< int, int >, HalfEdge*> m_Edges;
	std::vector<HENode*> m_PathNodes;
	std::vector<glm::vec3>						m_renderVertices_WALKABLE;
	std::vector<glm::vec3>						m_renderVertices_UNWALKABLE;
	std::vector<glm::vec3>						m_renderVertices_PATH;
public:
	NavMesh();
	void uploadToBuffer();
	~NavMesh();

	void Generate(const std::vector<glm::vec3>& vertices, 
		          const std::vector<unsigned int>& indices);
	void render();
	void Reset();
	HENode* GetNodeByRay(Ray* ray);


	float GetDistance(HENode* start, HENode* end);

	std::vector<HENode*> FindPath(HENode* start, HENode* end);

	inline const std::vector<HENode*>& GetNodes() const { return m_Nodes; }
};