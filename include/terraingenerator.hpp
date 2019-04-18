#pragma once
#include <glad/glad.h>
#include "mesh.hpp"

class Camera;
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
	inline float& getTileSize() { return _tileSize; }
	inline float& getTileHeight() { return _tileHeight; }
	inline int& getSelectedTile() { return _selectedTile; }
	void pickAgainstTerrain(std::shared_ptr<Camera> inCamera);
	bool _rayAgainstQuad(const glm::vec3& ray, const glm::vec3& rayOrigin);
private:
	std::shared_ptr<Mesh> _mesh;
	std::vector<Quad> _quads;
	int _selectedTile;
	int _numPatches;
	float _tileSize;
	float _tileHeight;
};