#include "model.hpp"

Model::Model() {
	
}

Model::~Model() {
	
}

void Model::addMesh(const Mesh& mesh) {
	_meshes.push_back(mesh);
}