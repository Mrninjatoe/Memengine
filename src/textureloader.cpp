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

// I hate this function so much, but too lazy to fix for now...
std::shared_ptr<Texture> TextureLoader::loadCubeMapTexture(const std::string& fileName) {
	auto textureAlreadyMade = _textures.count(fileName);
	if (textureAlreadyMade)
		return _textures[fileName];
	
	std::string filePath = "assets/textures/cubemaps/" + fileName + '/';
	std::vector<std::string> faces = {
		filePath + fileName + "_right.jpg",
		filePath + fileName + "_left.jpg",
		filePath + fileName + "_top.jpg",
		filePath + fileName + "_bottom.jpg",
		filePath + fileName + "_front.jpg",
		filePath + fileName + "_back.jpg"
	};
	
	_textures[fileName] = std::make_shared<Texture>();
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glm::ivec2 sizes = { 1,1 };
	for (unsigned int i = 0; i < 6; i++) {
		SDL_Surface* surface = IMG_Load(faces[i].c_str());
		if (surface == NULL) {
			printf("Unable to load image %s! SDL_image Error: %s\n", filePath.c_str(), IMG_GetError());
			abort();
		}
		else {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB,
				GL_UNSIGNED_BYTE, surface->pixels);
			sizes.x = surface->w;
			sizes.y = surface->h;
		}
		SDL_FreeSurface(surface);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	_textures[fileName]->initializeEmpty(textureID, sizes, Texture::TextureFormat::RGB32f);

	return _textures[fileName];
}