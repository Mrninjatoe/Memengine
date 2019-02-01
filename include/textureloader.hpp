#pragma once
#include <SDL2/SDL_image.h>
#include <string>
#include <map>
#include "texture.hpp"

class TextureLoader {
public:
	TextureLoader();
	~TextureLoader();
	Texture& loadTexture(const std::string& path);
private:
	std::map<const std::string, Texture> _textures;
};