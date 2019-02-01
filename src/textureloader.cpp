#include "textureloader.hpp"
#include "engine.hpp"

TextureLoader::TextureLoader() {
	
}

TextureLoader::~TextureLoader() {
	
}

Texture& TextureLoader::loadTexture(const std::string& path) {

	auto textureAlreadyMade = _textures.count(path);
	if (textureAlreadyMade)
		return _textures[path];

	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL)
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	else {
		optimizedSurface = SDL_ConvertSurface(loadedSurface, 
			Engine::getInstance()->getScreen()->getWindowSurface()->format, NULL);

		if (optimizedSurface == NULL)
			printf("Unable to optimize image %s! SDL_Error %s\n", path.c_str(), SDL_GetError());
			
		SDL_FreeSurface(loadedSurface);
	}

	Texture newTexture = Texture(optimizedSurface->w, optimizedSurface->h, optimizedSurface->pixels);

	//_textures[path] = newTexture;
	return newTexture;
}