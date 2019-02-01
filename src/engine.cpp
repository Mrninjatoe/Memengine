#include "engine.hpp"
#include <SDL2/SDL_events.h>
#include <glad/glad.h>

Engine* Engine::_instance;
Engine::Engine() {

}

int Engine::run() {
	_init();
	SDL_Event event;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0.0;

	bool quit = false;
	while (!quit) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = ((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
			}
		}
	}
	return 0;
}

void Engine::_init() {
	_initSDL();
	_initGL();

	_meshLoader = std::make_unique<MeshLoader>();
	_meshLoader->loadMesh("assets/models/duck.fbx");

	_normalShader = std::make_shared<ShaderProgram>("Test Pass");
	_normalShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/normal.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/normal.frag")
		.finalize();
}

void Engine::_initSDL() {
	_window = std::make_unique<Window>("Memengine");
}

void Engine::_initGL() {
	_renderer = std::make_unique<Renderer>(_window->getWindow());
}