#pragma once
#include "mesh.hpp"

class Model {
public:
	Model();
	virtual ~Model();
	Model& addMesh(const Mesh& mesh);
	std::vector<Mesh>& getMeshes() { return _meshes; }
private:
	std::vector<Mesh> _meshes;
};