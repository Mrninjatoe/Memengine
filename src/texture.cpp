#include <stdio.h>
#include "texture.hpp"

Texture::Texture() : _texture(0), _size(1,1), _format(RGBA8){

}

Texture::Texture(const TextureFormat& format, const int width, const int height, void* data) : 
	_format(format), _size(width, height){
	printf("Texture(format, width, height, data)\n");
	_setData(data);
}

Texture::Texture(const TextureFormat& format, const glm::ivec2& size, const bool& wantArray) : _format(format), _size(size) {
	printf("sunglasses\n");
	if (!wantArray)
		_fboTexture2D();
	else
		_fboTextureArray2D();
}

Texture::~Texture() {
	printf("~Texture()\n");
	glDeleteTextures(1, &_texture);
}