#include "NavMesh.h"
#include <climits>
#include <map>
#include <set>
#include <algorithm>

NavMesh::NavMesh():
	m_Shader(new Shader("Shaders/Default/NavMesh/NavMesh.vs", "Shaders/Default/NavMesh/NavMesh.frag"))
	,m_startNode(nullptr),m_endNode(nullptr)
{
	m_VAO[0] = 0; m_VAO[1] = 0; m_VAO[2] = 0; m_VAO[3] = 0; m_VAO[4] = 0;
	m_renderPartitions[0] = 0; m_renderPartitions[1] = 0;
}
void NavMesh::uploadWalkAbleUnwalkableAreas()
{
	if (m_VAO[0] || m_VAO[1]) {
		glDeleteVertexArrays(2, m_VAO);
		m_renderPartitions[0] = 0;
		m_renderPartitions[1] = 0;
		m_VAO[0] = 0; m_VAO[1] = 0; m_VAO[2] = 0;
	}
	m_renderVertices.clear();
	m_renderVertices.reserve(3 * (m_Nodes.size() + m_PathNodes.size()));
	for (HENode* _n : m_Nodes) {
		if (_n->IsWalkable()) {
			m_renderVertices.emplace_back(_n->GetVertices()[0]->GetPosition());
			m_renderVertices.emplace_back(_n->GetVertices()[1]->GetPosition());
			m_renderVertices.emplace_back(_n->GetVertices()[2]->GetPosition());
		}
	}
	m_renderPartitions[0] = m_renderVertices.size();
	for (HENode* const _n : m_Nodes) {
		if (!_n->IsWalkable()) {
			m_renderVertices.emplace_back(_n->GetVertices()[0]->GetPosition());
			m_renderVertices.emplace_back(_n->GetVertices()[1]->GetPosition());
			m_renderVertices.emplace_back(_n->GetVertices()[2]->GetPosition());
		}
	}
	m_renderPartitions[1] = m_renderVertices.size();
	GLuint VBO[2];

	glGenVertexArrays(2, m_VAO);
	glGenBuffers(2, VBO);

	///WALKABLE AREA
	glBindVertexArray(m_VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	// reserving buffer memory for walkable area
	glBufferData(GL_ARRAY_BUFFER, m_renderPartitions[0] * sizeof(glm::vec3), &m_renderVertices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	if (m_renderPartitions[0] != m_renderVertices.size()) {
		///UNWALKABLE AREA
		glBindVertexArray(m_VAO[1]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

		// reserving buffer memory for unwalkable area
		glBufferData(GL_ARRAY_BUFFER, (m_renderPartitions[1] - m_renderPartitions[0]) * sizeof(glm::vec3), &m_renderVertices[m_renderPartitions[0]], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

}
void NavMesh::uploadPathArea()
{
	if (m_VAO[2]) {
		glDeleteVertexArrays(1, &m_VAO[2]);
		 m_VAO[2] = 0;

	}
	m_renderVertices.erase(m_renderVertices.begin() + m_renderPartitions[1], m_renderVertices.end());//erase last path
	for (HENode* _n : m_PathNodes) {
		m_renderVertices.emplace_back(_n->GetVertices()[0]->GetPosition());
		m_renderVertices.emplace_back(_n->GetVertices()[1]->GetPosition());
		m_renderVertices.emplace_back(_n->GetVertices()[2]->GetPosition());
	}
	GLuint VBO;

	glGenVertexArrays(1, &m_VAO[2]);
	glGenBuffers(1, &VBO);

	if (m_renderPartitions[1] != m_renderVertices.size()) {
		///PATH AREA
		glBindVertexArray(m_VAO[2]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// reserving buffer memory for path area
		glBufferData(GL_ARRAY_BUFFER, (m_renderVertices.size() - m_renderPartitions[1]) * sizeof(glm::vec3), &m_renderVertices[m_renderPartitions[1]], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

}
void NavMesh::uploadStart()
{
	if (m_VAO[3]) {
		glDeleteVertexArrays(1, &m_VAO[3]);
		m_VAO[3] = 0;
	}


	if (m_startNode != nullptr) {
		m_startEndVertices[0] = m_startNode->GetVertices()[0]->GetPosition();
		m_startEndVertices[1] = m_startNode->GetVertices()[1]->GetPosition();
		m_startEndVertices[2] = m_startNode->GetVertices()[2]->GetPosition();

		GLuint VBO;

		glGenVertexArrays(1, &m_VAO[3]);
		glGenBuffers(1, &VBO);

		glBindVertexArray(m_VAO[3]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// reserving buffer memory for path area
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec3), &m_startEndVertices[0], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

}
void NavMesh::uploadEnd()
{
	if (m_VAO[4]) {
		glDeleteVertexArrays(1, &m_VAO[4]);
		m_VAO[4] = 0;
	}
	if (m_endNode != nullptr) {
		m_startEndVertices[3] = m_endNode->GetVertices()[0]->GetPosition();
		m_startEndVertices[4] = m_endNode->GetVertices()[1]->GetPosition();
		m_startEndVertices[5] = m_endNode->GetVertices()[2]->GetPosition();

		GLuint VBO;

		glGenVertexArrays(1, &m_VAO[4]);
		glGenBuffers(1, &VBO);
		glBindVertexArray(m_VAO[4]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// reserving buffer memory for path area
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec3), &m_startEndVertices[3], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

}
NavMesh::~NavMesh()
{
	Reset();
}

void NavMesh::SetStartNode(HENode * n) { m_startNode = n; }

void NavMesh::SetEndNode(HENode * n) { m_endNode = n; }

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
		radius *= 1.3;//a bit bigger radius to make selecting easier
		SphereCollider* collider = new SphereCollider(position, radius);
		node->SetCollider(collider);
	}

	for (const auto& edge : m_Edges)
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
	glBindVertexArray(m_VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, m_renderPartitions[0]);
	glBindVertexArray(0);
	//UNWALKABLE
	m_Shader->setVec3("col", glm::vec3(1.0f, 0.0f, 0.0f));
	glBindVertexArray(m_VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, (m_renderPartitions[1] - m_renderPartitions[0]));
	glBindVertexArray(0);
	//PATH
	 model=glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.08f, 0.0f));// shift over walkable cause all of its cells walkable
	m_Shader->setMat4("model", model);
	m_Shader->setVec3("col", glm::vec3(0.0f, 0.0f, 1.0f));
	glBindVertexArray(m_VAO[2]);
	glDrawArrays(GL_TRIANGLES, 0, (m_renderVertices.size()-m_renderPartitions[1]));
	glBindVertexArray(0);
	//Start AND END
	//START NODE
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.1f, 0.0f));// shift over PATH and WALKABLE
	m_Shader->setMat4("model", model);
	m_Shader->setVec3("col", glm::vec3(1.0f, .0f, 1.0f));
	glBindVertexArray(m_VAO[3]);
	glDrawArrays(GL_TRIANGLES, 0,3);
	glBindVertexArray(0);
	//END NODE
	m_Shader->setVec3("col", glm::vec3(0.0f, 1.0f, 1.0f));
	glBindVertexArray(m_VAO[4]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	Shader::Stop();
}

void NavMesh::Reset()
{
	m_startNode = nullptr;
	m_endNode = nullptr;
	m_renderVertices.clear();
	for (auto node : m_Nodes)
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
		if (node->GetCollider()->isIntersecting(ray)&&node->IsWalkable())//usually we pick walkable nodes
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
	std::swap(start, end);

	if (!start->IsWalkable() || !end->IsWalkable())
	{
		return std::vector<HENode*>();
	}

	HENode::AdvanceQuery();

	start->SetGCost(0);
	start->SetHCost(0);

	std::multiset<HENode*, Compare> open;
	open.insert(start);

	bool found = false;

	while (!open.empty())
	{
		auto it = open.begin();
		HENode* current = (*it);
		current->SetProcessed();
		open.erase(it);
		
		if (current == end)
		{
			found = true;
			break;
		}

		for (auto neighbour : current->GetWalkableNeighbors())
		{
			if (neighbour->IsProcessed())
			{
				continue;
			}
			
			float distanceToNeighbor = GetDistance(current, neighbour);
			float distanceToGoal = GetDistance(current, end);

			bool inOpenSet = open.find(neighbour) != open.end();

			if (current->GetGCost() + distanceToNeighbor < neighbour->GetGCost() || 
				!inOpenSet)
			{
				neighbour->SetGCost(current->GetGCost() + distanceToNeighbor);
				neighbour->SetHCost(distanceToGoal);

				neighbour->SetParent(current);

				if (!inOpenSet)
				{
					open.insert(neighbour);
				}
			}
		}
	}
	
	std::vector<HENode*> path;

	if (found)
	{
		HENode* current = end;

		while (current != start)
		{
			path.emplace_back(current);
			current = current->GetParent();
		}

		path.push_back(start);
	}

	m_PathNodes = path;
	
	return path;
}