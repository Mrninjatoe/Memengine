#pragma once
#include <glad/glad.h>
#include "mesh.hpp"

class TerrainGenerator{
public:
	struct Quad {
		Mesh::Vertex vertices[4];
		unsigned int indices[6];
	};

	TerrainGenerator();
	virtual ~TerrainGenerator();
	void initialize();

	inline std::shared_ptr<Mesh> getMesh() { return _mesh; }
	inline int getNumberOfTiles() { return _numPatches * _numPatches; }
private:
	std::shared_ptr<Mesh> _mesh;
	int _numPatches;
};