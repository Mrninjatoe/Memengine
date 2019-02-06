#pragma once
#include <string>
#include <map>
#include "texture.hpp"

class TextureLoader {
public:
	TextureLoader();
	~TextureLoader();
	std::shared_ptr<Texture> loadTexture(const std::string& path);
private:
	std::map<const std::string, std::shared_ptr<Texture>> _textures;
};