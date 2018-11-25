#include "NavMesh.h"
#include <climits>
#include <map>
#include <set>

NavMesh::NavMesh():
	m_Shader(new Shader("Shaders/Default/NavMesh/NavMesh.vs", "Shaders/Default/NavMesh/NavMesh.frag"))
,	m_VAO_WALKABLE(0)
, m_VAO_UNWALKABLE(0)
, m_VAO_PATH(0)
{
}


void NavMesh::uploadToBuffer()
{
	//TO DO CONFIGURE RESERVE FOR EACH VECTOR
	for (HENode* _n : m_Nodes) {
		if (_n->IsWalkable()) {
			m_renderVertices_WALKABLE.emplace_back(_n->GetVertices()[0]->GetPosition());
			m_renderVertices_WALKABLE.emplace_back(_n->GetVertices()[1]->GetPosition());
			m_renderVertices_WALKABLE.emplace_back(_n->GetVertices()[2]->GetPosition());
		}
		else {
			m_renderVertices_UNWALKABLE.emplace_back(_n->GetVertices()[0]->GetPosition());
			m_renderVertices_UNWALKABLE.emplace_back(_n->GetVertices()[1]->GetPosition());
			m_renderVertices_UNWALKABLE.emplace_back(_n->GetVertices()[2]->GetPosition());
		}
	}
	for (HENode* _n : m_PathNodes) {
		m_renderVertices_PATH.emplace_back(_n->GetVertices()[0]->GetPosition());
		m_renderVertices_PATH.emplace_back(_n->GetVertices()[1]->GetPosition());
		m_renderVertices_PATH.emplace_back(_n->GetVertices()[2]->GetPosition());
	}

	GLuint VBO_WALKABLE, VBO_UNWALKABLE, VBO_PATH;
	glGenVertexArrays(1, &m_VAO_WALKABLE);
	glGenBuffers(1, &VBO_WALKABLE);
	glBindVertexArray(m_VAO_WALKABLE);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_WALKABLE);

	// reserving buffer memory for walkable area
	glBufferData(GL_ARRAY_BUFFER, m_renderVertices_WALKABLE.size() * sizeof(glm::vec3), &m_renderVertices_WALKABLE[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	///UNWALKABLE AREA
	if (m_renderVertices_UNWALKABLE.size()>0) {
	glGenVertexArrays(1, &m_VAO_UNWALKABLE);
	glGenBuffers(1, &VBO_UNWALKABLE);
	glBindVertexArray(m_VAO_UNWALKABLE);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_UNWALKABLE);

	// reserving buffer memory for unwalkable area
	glBufferData(GL_ARRAY_BUFFER, m_renderVertices_UNWALKABLE.size() * sizeof(glm::vec3), &m_renderVertices_UNWALKABLE[0],GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
		}
	if (m_PathNodes.size()>0) {
		///PATH AREA
		glGenVertexArrays(1, &m_VAO_PATH);
		glGenBuffers(1, &VBO_PATH);
		glBindVertexArray(m_VAO_PATH);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_PATH);

		// reserving buffer memory for path area
		glBufferData(GL_ARRAY_BUFFER, m_renderVertices_PATH.size() * sizeof(glm::vec3), &m_renderVertices_PATH[0], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
}
NavMesh::~NavMesh()
{
	Reset();
}

void NavMesh::Generate(const std::vector<glm::vec3>&    vertices,
					   const std::vector<unsigned int>& indices)
{
	Reset();
	
	m_Nodes.reserve(vertices.size() / 3);

	HalfEdge* edge[3];
	HEVertex* vertex[3];
	
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		HENode* node = new HENode();
		m_Nodes.emplace_back(node);

		glm::vec3 position;

		for (int j = 0; j < 3; ++j)
		{
			edge[j] = new HalfEdge();
			edge[j]->SetNode(node);

			auto it = m_Vertices.find(indices[i + j]);
			
			if (it == m_Vertices.end())
			{
				m_Vertices[indices[i + j]] = vertex[j] = new HEVertex();
				vertex[j]->SetPosition(vertices[indices[i + j]]);
			}
			else
			{
				vertex[j] = it->second;
			}

			position += vertex[j]->GetPosition();
		}
		
		position /= 3;

		node->SetIsWalkable(true);
		node->SetPosition(position);
		node->SetHalfEdge(edge[0]);

		for (int j = 0; j < 3; ++j)
		{
			edge[j]->SetNext(edge[(j + 1) % 3]);
			edge[j]->SetPrevious(edge[((j - 1) + 3) % 3]);
			edge[j]->SetVertex(vertex[(j + 1) % 3]);

			if (vertex[j]->GetHalfEdge() == nullptr)
			{
				vertex[j]->SetHalfEdge(edge[((j - 1) + 3) % 3]);
			}

			m_Edges[std::make_pair(indices[i + j], indices[i + ((j + 1) % 3)])] = edge[j];
		}

		glm::vec3 ab = vertex[0]->GetPosition() - vertex[1]->GetPosition();
		glm::vec3 ac = vertex[0]->GetPosition() - vertex[2]->GetPosition();
		glm::vec3 bc = vertex[2]->GetPosition() - vertex[1]->GetPosition();

		// Getting the inner sphere radius of the current node
		float area2 = glm::length(glm::cross(ab, ac));
		float radius = area2 / (glm::length(ab) + glm::length(ac) + glm::length(bc));
		
		SphereCollider* collider = new SphereCollider(position, radius);
		node->SetCollider(collider);
	}

	for (auto edge : m_Edges)
	{
		auto it = m_Edges.find(std::make_pair(edge.first.second, edge.first.first));
		
		if (it != m_Edges.end())
		{
			edge.second->SetOppsite(it->second);
		}
	}

}

void NavMesh::render()
{
	m_Shader->use();
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.05f, 0.0f));// a bit shifted (to show plane below it)
	m_Shader->setVec3("viewPos", Camera::getInstance()->Position);
	m_Shader->setMat4("projection", Camera::getInstance()->getProjMatrix());
	m_Shader->setMat4("view", Camera::getInstance()->getViewMatrix());
	m_Shader->setMat4("model", model);
	//WALKABLE
	m_Shader->setVec3("col", glm::vec3(0.0f,1.0f,0.0f));
	glBindVertexArray(m_VAO_WALKABLE);
	glDrawArrays(GL_TRIANGLES, 0, m_renderVertices_WALKABLE.size());
	glBindVertexArray(0);
	//UNWALKABLE
	m_Shader->setVec3("col", glm::vec3(1.0f, 0.0f, 0.0f));
	glBindVertexArray(m_VAO_UNWALKABLE);
	glDrawArrays(GL_TRIANGLES, 0, m_renderVertices_UNWALKABLE.size());
	glBindVertexArray(0);
	//PATH
	 model=glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.08f, 0.0f));// shift over walkable cause all of its cells walkable
	m_Shader->setMat4("model", model);
	m_Shader->setVec3("col", glm::vec3(0.0f, 0.0f, 1.0f));
	glBindVertexArray(m_VAO_PATH);
	glDrawArrays(GL_TRIANGLES, 0, m_renderVertices_PATH.size());
	glBindVertexArray(0);
	Shader::Stop();

}

void NavMesh::Reset()
{
	for (auto& node : m_Nodes)
	{
		delete node;
	}

	for (auto& edge : m_Edges)
	{
		delete edge.second;
	}

	for (auto& vertex : m_Vertices)
	{
		delete vertex.second;
	}

	if (!m_Nodes.empty())
	{
		m_Nodes.clear();
	}

	if (!m_Edges.empty())
	{
		m_Edges.clear();
	}

	if (!m_Vertices.empty())
	{
		m_Vertices.clear();
	}
}

HENode* NavMesh::GetNodeByRay(Ray* ray)
{
	HENode* target = nullptr;

	for (const auto& node : m_Nodes)
	{
		if (node->GetCollider()->isIntersecting(ray))
		{
			target = node;
			break;
		}
	}

	return target;
}

float NavMesh::GetDistance(HENode* start, HENode* end)
{
	glm::vec3 a = start->GetPosition();
	glm::vec3 b = end->GetPosition();
	glm::vec3 ab = b - a;
	return glm::length(ab);
}

std::vector<HENode*> NavMesh::FindPath(HENode* start, HENode* end)
{
	if (!start->IsWalkable() || !end->IsWalkable())
	{
		return std::vector<HENode*>();
	}

	for (const auto& node : m_Nodes)
	{
		node->SetGCost(std::numeric_limits<float>::infinity());
		node->SetHCost(std::numeric_limits<float>::infinity());
	}

	start->SetGCost(0);
	start->SetHCost(0);

	std::set<HENode*, Compare> open, closed;
	open.insert(start);

	while (!open.empty())
	{
		auto it = open.begin();
		HENode* current = (*it);
		closed.insert(current);
		open.erase(it);
		
		if (current == end)
		{
			break;
		}

		for (auto neighbour : current->GetWalkableNeighbors())
		{
			if (closed.find(neighbour) != closed.end())
			{
				continue;
			}

			float distanceToNeighbor = GetDistance(current, neighbour);
			float distanceToGoal = GetDistance(current, end);

			if (current->GetGCost() + distanceToNeighbor < neighbour->GetGCost() || 
				open.find(neighbour) == open.end())
			{
				neighbour->SetGCost(current->GetGCost() + distanceToNeighbor);
				neighbour->SetHCost(distanceToGoal);

				neighbour->SetParent(current);

				if (open.find(neighbour) == open.end())
				{
					open.insert(neighbour);
				}
			}
		}
	}

	std::vector<HENode*> path;

	HENode* current = end;

	while (current != start)
	{
		path.emplace_back(current);
		current = current->GetParent();
	}

	path.push_back(start);

	std::reverse(path.begin(), path.end());
	m_PathNodes = path;
	return path;
}