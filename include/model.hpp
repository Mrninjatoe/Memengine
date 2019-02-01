#pragma once
#include "mesh.hpp"

class Model {
public:
	Model();
	virtual ~Model();
	void addMesh(const Mesh& mesh);
private:
	std::vector<Mesh> _meshes;
};