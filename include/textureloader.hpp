#pragma once
#include <string>
#include <map>
#include "texture.hpp"

class TextureLoader {
public:
	TextureLoader();
	~TextureLoader();
	std::shared_ptr<Texture> loadTexture(const std::string& fileName);
	std::shared_ptr<Texture> loadCubeMapTexture(const std::string& fileName);
private:
	std::map<const std::string, std::shared_ptr<Texture>> _textures;
};