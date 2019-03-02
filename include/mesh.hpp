#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>
#include <memory>
#include "texture.hpp"

class Mesh {
public:
	const enum BindingLocation : const int {
		position = 0,
		normal = 1,
		color = 2,
		tangent = 3,
		uv = 4,
	};

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec3 tangent;
		glm::vec2 uv;
	};

	/*struct Material {
		
	};*/

	Mesh() {}; // Not used but just defined.
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	virtual ~Mesh();

	GLuint& getVAO() { return _vao; }
	std::vector<unsigned int>& getIndices() { return _indices; }
	std::vector<Vertex>& getVertices() { return _vertices; }
	void addTexture(const std::shared_ptr<Texture>& tex);
	std::vector<std::shared_ptr<Texture>> getTextures() { return _textures; }
	bool hasTangents() { return _hasTangents; }
	void setHasTangents(const bool& condition) { _hasTangents = condition; }

private:
	GLuint _vao, _vbo, _ebo;
	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;
	bool _hasTangents;

	std::vector<std::shared_ptr<Texture>> _textures;

	void _setupBuffers();
	void _setupAttributes();
};