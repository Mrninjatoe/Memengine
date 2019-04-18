#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>
#include <memory>
#include "texture.hpp"

class Mesh {
public:
	enum class BindingLocation : int {
		position = 0,
		normal = 1,
		color = 2,
		tangent = 3,
		uv = 4,
		m = 5,
		m_1 = 6,
		m_2 = 7,
		m_3 = 8,
		terrainOffset = 9
	};

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec3 tangent;
		glm::vec2 uv;
	};

	enum class TextureLocation : int {
		diffuse = 0,
		normalMap = 1,
		heightMap = 2
	};

	/*struct Material {
		
	};*/

	Mesh() {}; // Not used but just defined.
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	virtual ~Mesh();

	void addTexture(const std::shared_ptr<Texture> tex);
	void setupInstancedBuffer(const std::vector<glm::mat4>& matrices);
	void setupInstancedBuffer(const std::vector<glm::vec3>& offsets);
	void updateInstancedBufferFull(const std::vector<glm::mat4>& matrices);
	void updateInstancedBufferOffset(const glm::ivec2& offset, const glm::mat4& newMatrix);
	GLuint& getVAO() { return _vao; }
	std::vector<unsigned int>& getIndices() { return _indices; }
	std::vector<Vertex>& getVertices() { return _vertices; }
	std::vector<std::shared_ptr<Texture>> getTextures() { return _textures; }
	bool hasTangents() { return _hasTangents; }
	void setHasTangents(const bool& condition) { _hasTangents = condition; }
	bool hasParallax() { return _hasParallax; }
	void setHasParallax(const bool& condition) { _hasParallax = condition; }

private:
	GLuint _vao, _vbo, _ebo;
	GLuint _extraBuffer;
	std::vector<GLuint> _extrabuffers;
	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;
	bool _hasTangents;
	bool _hasParallax;

	std::vector<std::shared_ptr<Texture>> _textures;

	void _setupBuffers();
	void _setupAttributes();
};