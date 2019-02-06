#include <stdio.h>
#include "texture.hpp"

Texture::Texture() : _texture(0), _size(0,0), _format(RGBA8){
	printf(">:(\n");
	_setData(NULL);
}

Texture::Texture(const TextureFormat& format, const int width, const int height, void* data) : 
	_format(format), _size(width, height){
	_setData(data);
}

Texture::~Texture() {
	
}