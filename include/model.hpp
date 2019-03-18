#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "mesh.hpp"


class Model {
public:
	Model();
	virtual ~Model();
	void addMesh(const std::shared_ptr<Mesh> inMesh);
	std::vector<std::shared_ptr<Mesh>>& getMeshes();
	glm::mat4 getModelMatrix() { return glm::translate(_pos) * glm::mat4_cast(_orientation) * glm::scale(_scale); }
	Model& setPosition(const glm::vec3& pos) {
		_pos = pos;
		return *this;
	}
	Model& setRotation(const float& angle, const glm::vec3& rotation) {
		_rotation = glm::rotate(_rotation, glm::radians(angle), rotation);
		return *this;
	}
	Model& setScaling(const glm::vec3& scale) {
		_prevScale = _scale;
		_scale = scale;
		return *this;
	}
	float getRadius() {
		return _radius;
	}
	Model& applyPosition(const glm::vec3& translation) {
		_pos += translation;
		return *this;
	}
	Model& applyTempScaling(const glm::vec3& scale) {
		_scale = scale;
		return *this;
	}
	Model& applyOrientation(const glm::quat& inOrientation) {
		_orientation = inOrientation * _orientation;
		return *this;
	}
	glm::vec3 getScaling() {
		return _scale;
	}
	glm::vec3 getPrevScaling() {
		return _prevScale;
	}
	void setPrevScaling(const glm::vec3& newScale) {
		_prevScale = newScale;
	}
	unsigned int getInstanceCount() { return _instanceCount; }
	bool sphereAgainstRay(const glm::vec3& rayDir, const glm::vec3& rayOrigin);
	void makeInstanceable(const std::vector<glm::mat4>& matrices);

private:
	float _radius;
	bool _isInstanced;
	unsigned int _instanceCount;
	std::vector<std::shared_ptr<Mesh>> _meshes;
	glm::vec3 _pos;
	glm::vec3 _scale;
	glm::vec3 _prevScale;
	glm::mat4 _rotation;
	glm::quat _orientation;
};