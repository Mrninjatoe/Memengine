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
	Model& applyScaling(const glm::vec3& scale) {
		_scale += scale;
		return *this;
	}
	Model& applyScaleToX(const float& xAxis) {
		_scale = glm::vec3(xAxis, _scale.y, _scale.z);
		return *this;
	}
	Model& applyScaleToY(const float& yAxis) {
		_scale = glm::vec3(_scale.x, yAxis, _scale.z);
		return *this;
	}
	Model& applyScaleToZ(const float& zAxis) {
		_scale = glm::vec3(_scale.x, _scale.y, zAxis);
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
	bool sphereAgainstRay(const glm::vec3& rayDir, const glm::vec3& rayOrigin);

private:
	float _radius;
	std::vector<std::shared_ptr<Mesh>> _meshes;
	glm::vec3 _pos;
	glm::vec3 _scale;
	glm::vec3 _prevScale;
	glm::mat4 _rotation;
	glm::quat _orientation;
};