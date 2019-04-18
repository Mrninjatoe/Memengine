#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "mesh.hpp"

class Model {
public:
	Model();
	Model(const std::string& identifierName);
	virtual ~Model();
	void addMesh(const std::shared_ptr<Mesh> inMesh);
	std::vector<std::shared_ptr<Mesh>> getMeshes();
	glm::mat4 getModelMatrix() { return glm::translate(_pos) * glm::mat4_cast(_orientation) * glm::scale(_scale); }
	Model& setPosition(const glm::vec3& pos) {
		_pos = pos;
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
	glm::vec3 getPosition() {
		return _pos;
	}
	void setPrevScaling(const glm::vec3& newScale) {
		_prevScale = newScale;
	}
	bool isInstanced() const { return _isInstanced; }
	bool sphereAgainstRay(const glm::vec3& rayDir, const glm::vec3& rayOrigin);
	void makeInstanceable(const std::vector<glm::mat4>& matrices);
	void updateInstanceInfo(const std::vector<glm::mat4>& matrices);

private:
	std::string _meshIdentifier;
	float _radius;
	bool _isInstanced;
	std::vector<std::shared_ptr<Mesh>> _meshes;
	glm::vec3 _pos;
	glm::vec3 _scale;
	glm::vec3 _prevScale;
	glm::quat _orientation;
};