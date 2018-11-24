#pragma once

#include <glm/glm.hpp>

class HalfEdge;

class HEVertex
{
private:
	glm::vec3 m_Position;
	HalfEdge *m_Edge;

public:
	HEVertex();
	~HEVertex();

	void SetPosition(const glm::vec3& position);
	void SetHalfEdge(HalfEdge* edge);

	inline const glm::vec3& GetPosition() const { return m_Position; }
	inline HalfEdge*  GetHalfEdge()       const { return m_Edge;     }
};