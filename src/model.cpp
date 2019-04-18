#include "model.hpp"

Model::Model() : _pos(0,0,0), _scale(1), _radius(5.f), _orientation(glm::quat(glm::vec3(0,0,0))), _prevScale(_scale),
	_isInstanced(false),  _meshIdentifier(""){

}

Model::Model(const std::string& identifierName) : _pos(0, 0, 0), _scale(1), _radius(5.f), _orientation(glm::quat(glm::vec3(0, 0, 0))), _prevScale(_scale),
_isInstanced(false),  _meshIdentifier(identifierName) {

}

Model::~Model() {
	_meshes.clear();
}

void Model::addMesh(const std::shared_ptr<Mesh> inMesh) {
	_meshes.push_back(inMesh);
}

std::vector<std::shared_ptr<Mesh>> Model::getMeshes() {
	return _meshes;
}

// Only Sphere intersection test for now.
// Radius is also hard-coded, should add hitboxes in blender.
bool Model::sphereAgainstRay(const glm::vec3& rayDir, const glm::vec3& rayOrigin) {
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

void Model::makeInstanceable(const std::vector<glm::mat4>& matrices) {
	_isInstanced = true;
	for (auto mesh : _meshes)
		mesh->setupInstancedBuffer(matrices);
}

void Model::updateInstanceInfo(const std::vector<glm::mat4>& matrices) {
	for (auto mesh : _meshes)
		mesh->updateInstancedBufferFull(matrices);
}