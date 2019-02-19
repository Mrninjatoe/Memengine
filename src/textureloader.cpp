#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "textureloader.hpp"
#include "engine.hpp"

TextureLoader::TextureLoader() {

}

TextureLoader::~TextureLoader() {
	printf("~TextureLoader()\n");
	_textures.clear();
}

std::shared_ptr<Texture> TextureLoader::loadTexture(const std::string& fileName) {
	std::string filePath = "assets/textures/" + fileName;
	printf("%s\n", filePath.c_str());
	auto textureAlreadyMade = _textures.count(filePath);
	if (textureAlreadyMade)
		return _textures[filePath];

	SDL_Surface* surface = IMG_Load(filePath.c_str());

	if (surface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", filePath.c_str(), IMG_GetError());
		abort();
	}
	else {
		SDL_Surface* optimizedSurface = NULL;
		optimizedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
		if (optimizedSurface == NULL) {
			printf("Unable to optimize image %s! SDL_Error %s\n", filePath.c_str(), SDL_GetError());
			abort();
		}
		
		SDL_FreeSurface(surface);
		surface = optimizedSurface;	
	}

	_textures[filePath] = std::make_shared<Texture>(Texture::TextureFormat::RGBA8, surface->w, surface->h, surface->pixels);

	SDL_FreeSurface(surface);
	return _textures[filePath];
}