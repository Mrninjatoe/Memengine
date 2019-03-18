#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) : _hasTangents(false),
_hasParallax(false){
	_vertices = vertices;
	_indices = indices;
	_setupBuffers();
	_setupAttributes();
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_vao);
	_textures.clear();
	printf("~Mesh()\n");
}

void Mesh::_setupBuffers() {
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);	
}

void Mesh::_setupAttributes() {
	glBindVertexArray(_vao);

	glEnableVertexAttribArray(BindingLocation::position);
	glVertexAttribPointer(BindingLocation::position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	glEnableVertexAttribArray(BindingLocation::normal);
	glVertexAttribPointer(BindingLocation::normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::normal));

	glEnableVertexAttribArray(BindingLocation::color);
	glVertexAttribPointer(BindingLocation::color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::color));

	glEnableVertexAttribArray(BindingLocation::tangent);
	glVertexAttribPointer(BindingLocation::tangent, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::tangent));

	glEnableVertexAttribArray(BindingLocation::uv);
	glVertexAttribPointer(BindingLocation::uv, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::uv));

	glBindVertexArray(0);
}

void Mesh::setupInstancedBuffer(const std::vector<glm::mat4>& matrices) {
	glBindVertexArray(_vao);

	glGenBuffers(1, &_extraBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _extraBuffer);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &matrices[0], GL_STATIC_DRAW); // Might change to stream_draw later. :)

	auto vec4Size = sizeof(glm::vec4);

	glEnableVertexAttribArray(BindingLocation::m);
	glVertexAttribPointer(BindingLocation::m, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (GLvoid*)0);

	glEnableVertexAttribArray(BindingLocation::m_1);
	glVertexAttribPointer(BindingLocation::m_1, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (GLvoid*)vec4Size);

	glEnableVertexAttribArray(BindingLocation::m_2);
	glVertexAttribPointer(BindingLocation::m_2, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (GLvoid*)(vec4Size * 2));

	glEnableVertexAttribArray(BindingLocation::m_3);
	glVertexAttribPointer(BindingLocation::m_3, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (GLvoid*)(vec4Size * 3));
	
	glVertexAttribDivisor(BindingLocation::m, 1);
	glVertexAttribDivisor(BindingLocation::m_1, 1);
	glVertexAttribDivisor(BindingLocation::m_2, 1);
	glVertexAttribDivisor(BindingLocation::m_3, 1);

	glBindVertexArray(0);
}

void Mesh::addTexture(const std::shared_ptr<Texture>& tex) {
	_textures.push_back(tex);
}