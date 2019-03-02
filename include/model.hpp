#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "mesh.hpp"


class Model {
public:
	Model();
	virtual ~Model();
	void addMesh(const std::shared_ptr<Mesh> inMesh);
	std::vector<std::shared_ptr<Mesh>>& getMeshes();
	glm::mat4 getModelMatrix() { return glm::translate(_pos) * _rotation * glm::scale(_scale); }
	Model& setPosition(const glm::vec3& pos) {
		_pos = pos;
		return *this;
	}
	Model& setRotation(const float& angle, const glm::vec3& rotation) {
		_rotation = glm::rotate(_rotation, glm::radians(angle), rotation);
		return *this;
	}
	Model& setScaling(const glm::vec3& scale) {
		_scale = scale;
		return *this;
	}
	float getRadius() {
		return _radius;
	}

	bool testAgainstRay(const glm::vec3& rayDir, const glm::vec3& rayOrigin);

private:
	float _radius;
	std::vector<std::shared_ptr<Mesh>> _meshes;
	glm::vec3 _pos;
	glm::vec3 _scale;
	glm::mat4 _rotation;
};