#include <stdio.h>
#include <SDL2/SDL.h>
#include <glm/gtx/transform.hpp>
#include "engine.hpp"
#include "camera.hpp"

Camera::Camera() {
	pos = {0,0,0};
	cameraTarget = { 0,0,0 };
	cameraUp = { 0,1,0 };

	direction = glm::normalize(pos - cameraTarget);
	cameraRight = glm::normalize(glm::cross(cameraUp, direction));
	cameraUp = glm::cross(direction, cameraRight);
	auto size = Engine::getInstance()->getWindow()->getSize();
	aspectRatio = size.x / (float)size.y;
	
	timeCounter = 0.f;
}

Camera::~Camera() {
	printf("~Camera()\n");
}

void Camera::update(float dt) {
	if (!enableMouse) {
		glm::ivec2 dm = mousePos;
		SDL_GetMouseState(&mousePos.x, &mousePos.y);
		auto sizes = Engine::getInstance()->getWindow()->getSize();
		if (dm != mousePos) {
			mousePos -= sizes / 2;

			pitch += dm.y * 0.001f;
			yaw -= dm.x * 0.001f;
			float hpi = glm::half_pi<float>() - 0.001f;
			pitch = glm::clamp(pitch, -hpi, hpi);
			SDL_WarpMouseInWindow(Engine::getInstance()->getWindow()->getWindow(), sizes.x / 2, sizes.y / 2);
		}
	}
	else {
		if (leftClick) {
			SDL_GetMouseState(&mousePos.x, &mousePos.y);
			auto sizes = Engine::getInstance()->getWindow()->getSize();
			// Convert mousePos to center of screen and inbetween -1 - 1.
			//glm::vec2 tempMousePos = glm::vec2((mousePos.x * 2.f) / sizes.x - 1.f, 1.f - (2.f * mousePos.y) / sizes.y);
			// Create a ray going forward 
			glm::vec4 rayClip = glm::vec4((mousePos.x * 2.f) / sizes.x - 1.f, 1.f - (2.f * mousePos.y) / sizes.y, -1.f, 1.f);
			glm::vec4 rayEye = glm::inverse(this->getProjectionMatrix()) * rayClip;
			rayEye.z = -1; // Forward in world space.
			rayEye.w = 0.f; // not a point.
			glm::vec3 rayWorld = glm::normalize(glm::inverse(this->getViewMatrix()) * rayEye);
			auto models = Engine::getInstance()->getWorldObjects();
			for (auto model : models) {
				if (model->testAgainstRay(rayWorld, this->pos)) {
					currentTarget = model;
					model->setScaling(glm::vec3(5));
					break;
				}
			}
		}
	}
	float speed = 10.f;
	if (pressedShift)
		speed *= 10;

	glm::vec3 walk_dir = {0,0,0};
	if (moveForward)
		walk_dir += glm::vec3(0, 0, 1);

	if (moveBack)
		walk_dir += glm::vec3(0, 0, -1);

	if (moveLeft)
		walk_dir += glm::vec3(1, 0, 0);

	if (moveRight)
		walk_dir += glm::vec3(-1, 0, 0);

	orientation = glm::quat(glm::vec3(0, yaw, 0)) * glm::quat(glm::vec3(pitch, 0, 0));
	walk_dir = orientation * walk_dir;
	pos += walk_dir * speed * dt;
	if (moveUp)
		pos.y += speed * dt;
	
	if (moveDown)
		pos.y -= speed * dt;

	timeCounter += 1 * dt;
}

glm::mat4 Camera::getViewMatrix() {
	return glm::inverse(glm::translate(pos) * glm::mat4_cast(glm::rotate(orientation, glm::pi<float>(), glm::vec3(0, 1, 0))));
}

glm::mat4 Camera::getProjectionMatrix() {
	return glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
}