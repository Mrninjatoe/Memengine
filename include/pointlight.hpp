#pragma once
#include <glm/vec3.hpp>

class Pointlight {
public:
	Pointlight();
	Pointlight(const glm::vec3& inPos, const glm::vec3& inColor);
	virtual ~Pointlight();

	glm::vec3 pos;
	glm::vec3 color;
	float radius;
};