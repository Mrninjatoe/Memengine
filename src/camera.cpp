#define GLM_ENABLE_EXPERIMENTAL
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

	view = glm::lookAt(pos, cameraTarget, cameraUp);
	timeCounter = 0.f;
	walkDir = { 0,0,0 };
}

Camera::~Camera() {
	
}

void Camera::update(float dt) {
	if (!enableMouse) {
		glm::ivec2 dm = mousePos;
		SDL_GetMouseState(&mousePos.x, &mousePos.y);
		if (dm != mousePos) {
			auto sizes = Engine::getInstance()->getWindow()->getSize();
			mousePos -= sizes / 2;

			pitch += dm.y * 0.001f;
			yaw -= dm.x * 0.001f;
			float hpi = glm::half_pi<float>() - 0.001;
			pitch = glm::clamp(pitch, -hpi, hpi);
			SDL_WarpMouseInWindow(Engine::getInstance()->getWindow()->getWindow(), sizes.x / 2, sizes.y / 2);
		}
	}
	float speed = 350.f;
	if (pressedShift)
		speed *= 10;

	glm::vec3 walk_dir;
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

	pos += walk_dir * dt * speed * 0.001f;
	if (moveUp)
		pos.y += speed * dt;

	if (moveDown)
		pos.y -= speed * dt;

	timeCounter += 1 * dt;
}

void Camera::_resetData() {
	walkDir = { 0,0,0 };
	speedMultipler = 1.0f;
}

glm::mat4 Camera::getViewMatrix() {
	return glm::inverse(glm::translate(pos) * glm::mat4_cast(glm::rotate(orientation, glm::pi<float>(), glm::vec3(0, 1, 0))));
}

glm::mat4 Camera::getProjectionMatrix() {
	auto size = Engine::getInstance()->getWindow()->getSize();
	float aspect = size.x / (float)size.y;

	return glm::perspective(glm::radians(70.f), aspect, zNear, zFar);
}