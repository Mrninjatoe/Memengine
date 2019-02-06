#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
	const enum BindingLocation : const int {
		position = 0,
		normal = 1,
		color = 2,
		uv = 3,
	};

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 uv;
	};

	Mesh() {}; // Not used but just defined.
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	virtual ~Mesh();

	GLuint& getVAO() { return _vao; }
	std::vector<unsigned int>& getIndices() { return _indices; }
	std::vector<Vertex>& getVertices() { return _vertices; }

private:
	GLuint _vao, _vbo, _ebo;
	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;

	void _setupBuffers();
	void _setupAttributes();
};