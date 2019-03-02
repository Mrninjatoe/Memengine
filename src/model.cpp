#include "model.hpp"

Model::Model() : _pos(0,0,0), _scale(1), _radius(5.f){
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

// Sphere intersection test right now.
bool Model::testAgainstRay(const glm::vec3& rayDir, const glm::vec3& rayOrigin) {
	double det, b;
	glm::vec3 p = rayOrigin - _pos;
	b = -glm::dot(rayDir, p);
	det = b * b - glm::dot(p,p) + _radius * _radius;
	if (det < 0)
		return false;

	det = sqrt(det);
	double intersect1, intersect2;
	intersect1 = b - det;
	intersect2 = b + det;
	if (intersect2 < 0)
		return false;
	if (intersect1 < 0)
		intersect1 = 0;

	return true;
}