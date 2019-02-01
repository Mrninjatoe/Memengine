#include <SDL2/SDL_image.h>
#include <glad/glad.h>
#include "texture.hpp"

Texture::Texture() {
	_size.x = 0;
	_size.y = 0;
}

Texture::Texture(const int& width, const int& height, void* data) : _size(width, height){
	_setData(data);
}

Texture::~Texture() {
	
}

void Texture::_setData(void* pixels) {
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _size.x, _size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
}