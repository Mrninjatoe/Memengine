#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>
#include "camera.hpp"
constexpr auto NUM_CASCADE_SPLITS = 4;

class Shadowcaster {
public:
	Shadowcaster();
	virtual ~Shadowcaster();
	void update(float dt);
	void createCascadeSplits(const std::shared_ptr<Camera>& playerCamera, const int& texSize);
	Shadowcaster& setPosition(const glm::vec3& pos);
	Shadowcaster& setRotation(const float& angle, const glm::vec3& axis);
	Shadowcaster& lookAt(const glm::vec3& targetPos);
	Shadowcaster& setNearBox(const glm::vec2& nearBox);
	Shadowcaster& setFarBox(const glm::vec2& farBox);
	Shadowcaster& setNearPlane(const float& nearPlane);
	Shadowcaster& setFarPlane(const float& farPlane);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getLightspaceMatrix();
	glm::vec3 getPos();
	glm::mat4 getViewProjMatrix(const int& pos);
	glm::vec4 getCascadedSplits();
	void makeStatic(const bool& condition);
	bool& isStatic() { return _static; }

	float lambda = 1.f;
	float minDistance = 0.f;
	float maxDistance = 0.016f;
private:
	bool _static = true;
	glm::vec3 _pos;
	glm::mat4 _rotation;
	float _cascadedSplits[NUM_CASCADE_SPLITS];
	glm::mat4 _cascadedViewProjs[NUM_CASCADE_SPLITS];
	glm::vec3 _lookAtTarget;
	glm::vec2 _nearBox;
	glm::vec2 _farBox;
	float _nearPlane;
	float _farPlane;
	float _timeCounter;
};