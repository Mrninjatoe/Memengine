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

void Texture::intializeVSMTex(const glm::ivec2& sizes, const int& numOfLayers) {
	_format = TextureFormat::RG32f;
	_size = sizes;

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _texture);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float amount = 4.f;
	float supportedMax = 0.f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &supportedMax);
	amount = glm::min(amount, supportedMax);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, toGLInternal(_format),
		_size.x, _size.y, (GLsizei)numOfLayers);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}