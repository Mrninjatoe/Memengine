#include "model.hpp"

Model::Model() {
	
}

Model::~Model() {
	
}

Model& Model::addMesh(const Mesh& mesh) {
	_meshes.push_back(mesh);
	
	return *this;
}