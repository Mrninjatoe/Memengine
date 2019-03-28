#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/vec2.hpp>
#undef main

class Window {
public:
	Window(const std::string& caption);
	virtual ~Window();
	SDL_Window* getWindow() { return _window; }
	SDL_Surface* getWindowSurface() { return SDL_GetWindowSurface(_window); }
	glm::ivec2 getSize() { return glm::ivec2(_screenWidth, _screenHeight); }

private:
	SDL_Window* _window;
	int _screenWidth = 1280;
	int _screenHeight = 720;
	bool _fullscreen = false;
};