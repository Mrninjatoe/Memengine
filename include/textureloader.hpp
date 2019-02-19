#pragma once
#include <string>
#include <map>
#include "texture.hpp"

class TextureLoader {
public:
	TextureLoader();
	~TextureLoader();
	std::shared_ptr<Texture> loadTexture(const std::string& fileName);
	/*std::shared_ptr<Texture> loadEmbeddedTexture(const unsigned int& width, const unsigned int& height, void* data, const std::string& filePath);
	std::shared_ptr<Texture> loadEmbeddedTexture(const char* ext, uint32_t size, void* data, const std::string filePath);*/
private:
	std::map<const std::string, std::shared_ptr<Texture>> _textures;
};