#pragma once
#include <glm/glm.hpp>

class Texture {
public:
	Texture();
	Texture(const int& width, const int& height, void* data);
	~Texture();
	void _setData(void* pixels);
private:
	glm::vec2 _size;
	unsigned int _texture;
};