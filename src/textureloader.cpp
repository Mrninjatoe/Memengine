#include <SDL2/SDL_image.h>
#include "textureloader.hpp"
#include "engine.hpp"

TextureLoader::TextureLoader() {
	
}

TextureLoader::~TextureLoader() {
	
}

std::shared_ptr<Texture> TextureLoader::loadTexture(const std::string& path) {

	auto textureAlreadyMade = _textures.count(path);
	if (textureAlreadyMade)
		return _textures[path];

	SDL_Surface* surface = IMG_Load(path.c_str());

	if (surface == NULL)
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	else {
		SDL_Surface* optimizedSurface = NULL;
		optimizedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);

		if (optimizedSurface == NULL)
			printf("Unable to optimize image %s! SDL_Error %s\n", path.c_str(), SDL_GetError());
		
		surface = optimizedSurface;	
	}

	_textures[path] = std::make_shared<Texture>(Texture::TextureFormat::RGBA8, surface->w, surface->h, surface->pixels);

	SDL_FreeSurface(surface);
	return _textures[path];
}