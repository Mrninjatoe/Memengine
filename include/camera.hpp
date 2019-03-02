#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "model.hpp"

class Camera {
public:
	Camera();
	virtual ~Camera();
	void update(float deltaTime);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	glm::vec3 pos;
	glm::vec3 cameraTarget;
	glm::vec3 direction;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;
	std::shared_ptr<Model> currentTarget;

	glm::ivec2 mousePos = { 0,0 };
	
	float fov = 70.f;
	float zNear = 1.f;
	float zFar = 10000.f;
	float pitch = 0.f;
	float yaw = 0.f;
	float speedMultipler = 1.0f;
	float aspectRatio = 4 / 3.f;
	
	glm::quat orientation;

	bool pressedShift = false;
	bool moveUp = false;
	bool moveDown = false;
	bool moveForward = false;
	bool moveBack = false;
	bool moveRight = false;
	bool moveLeft = false;
	bool enableMouse = true;
	bool leftClick = false;

	float timeCounter;
};


//float radius = 2.f;
//float camX = sin(_timeCounter) * radius;
//float camZ = cos(_timeCounter) * radius;
//
//_pos.x = camX;
//_pos.z = camZ;
//
//_timeCounter += 1 * deltaTime;