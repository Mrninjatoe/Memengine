#pragma once
#include <glad/glad.h>
#include <vector>
#include <memory>
#include "pointlight.hpp"

class Uniformbuffer {
public:
	Uniformbuffer();
	virtual ~Uniformbuffer();
	template<typename T>
	void setData(const std::vector<std::shared_ptr<T>> inData) {
		printf("Size of T: %zu\n", sizeof(T));
		glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * inData.size(), &inData, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
private:
	GLuint _ubo;
};