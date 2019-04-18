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

	glEnableVertexAttribArray((GLint)BindingLocation::position);
	glVertexAttribPointer((GLint)BindingLocation::position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	glEnableVertexAttribArray((GLint)BindingLocation::normal);
	glVertexAttribPointer((GLint)BindingLocation::normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::normal));

	glEnableVertexAttribArray((GLint)BindingLocation::color);
	glVertexAttribPointer((GLint)BindingLocation::color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::color));

	glEnableVertexAttribArray((GLint)BindingLocation::tangent);
	glVertexAttribPointer((GLint)BindingLocation::tangent, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::tangent));

	glEnableVertexAttribArray((GLint)BindingLocation::uv);
	glVertexAttribPointer((GLint)BindingLocation::uv, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::uv));

	glBindVertexArray(0);
}

void Mesh::setupInstancedBuffer(const std::vector<glm::mat4>& matrices) {
	glBindVertexArray(_vao);

	glGenBuffers(1, &_extraBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _extraBuffer);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &matrices[0], GL_DYNAMIC_DRAW); // Might change to stream_draw later. :)

	auto vec4Size = sizeof(glm::vec4);

	glEnableVertexAttribArray((GLint)BindingLocation::m);
	glVertexAttribPointer((GLint)BindingLocation::m, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (GLvoid*)0);

	glEnableVertexAttribArray((GLint)BindingLocation::m_1);
	glVertexAttribPointer((GLint)BindingLocation::m_1, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (GLvoid*)vec4Size);

	glEnableVertexAttribArray((GLint)BindingLocation::m_2);
	glVertexAttribPointer((GLint)BindingLocation::m_2, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (GLvoid*)(vec4Size * 2));

	glEnableVertexAttribArray((GLint)BindingLocation::m_3);
	glVertexAttribPointer((GLint)BindingLocation::m_3, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (GLvoid*)(vec4Size * 3));
	
	glVertexAttribDivisor((GLint)BindingLocation::m, 1);
	glVertexAttribDivisor((GLint)BindingLocation::m_1, 1);
	glVertexAttribDivisor((GLint)BindingLocation::m_2, 1);
	glVertexAttribDivisor((GLint)BindingLocation::m_3, 1);

	glBindVertexArray(0);
}

void Mesh::setupInstancedBuffer(const std::vector<glm::vec3>& offsets) {
	glBindVertexArray(_vao);

	glGenBuffers(1, &_extraBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _extraBuffer);
	glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), &offsets[0], GL_DYNAMIC_DRAW); // Might change to stream_draw later. :)

	auto vec3Size = sizeof(glm::vec3);
	glEnableVertexAttribArray((GLint)BindingLocation::terrainOffset);
	glVertexAttribPointer((GLint)BindingLocation::terrainOffset, 3, GL_FLOAT, GL_FALSE, (GLsizei)vec3Size, (GLvoid*)0);
	glVertexAttribDivisor((GLint)BindingLocation::terrainOffset, 1);

	glBindVertexArray(0);
}

void Mesh::updateInstancedBufferFull(const std::vector<glm::mat4>& matrices) {
	glBindBuffer(GL_ARRAY_BUFFER, _extraBuffer);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &matrices[0], GL_DYNAMIC_DRAW);
}

void Mesh::updateInstancedBufferOffset(const glm::ivec2& offset, const glm::mat4& matrix) {
	
}


void Mesh::addTexture(const std::shared_ptr<Texture> tex) {
	_textures.push_back(tex);
}