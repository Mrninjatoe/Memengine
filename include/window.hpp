#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#undef main

class Window {
public:
	Window(const std::string& caption);
	virtual ~Window();
	SDL_Window* getWindow() { return _window; }
	SDL_Surface* getWindowSurface() { return SDL_GetWindowSurface(_window); }
private:
	SDL_Window* _window;
	float _screenWidth = 1280.f;
	float _screenHeight = 720.f;
	bool _fullscreen = false;

};