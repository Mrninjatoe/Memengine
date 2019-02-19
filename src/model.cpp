#include "model.hpp"

Model::Model() : _pos(0,0,0), _scale(1){
	_rotation = glm::rotate(0.f, glm::vec3(0,1,0));
}

Model::~Model() {
	_meshes.clear();
	printf("~Model()\n");
}

void Model::addMesh(const std::shared_ptr<Mesh> inMesh) {
	_meshes.push_back(inMesh);
	printf("Pushing mesh\n");
}

std::vector<std::shared_ptr<Mesh>>& Model::getMeshes() {
	return _meshes;
}