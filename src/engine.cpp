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
	float deltaTime = 0.0;

	bool quit = false;
	while (!quit) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = ((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;

		while (SDL_PollEvent(&event)) {
			{
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;
				case SDLK_LSHIFT:
					_camera->pressedShift = true;
					break;
				case SDLK_SPACE:
					_camera->moveUp = true;
					break;
				case SDLK_w:
					_camera->moveForward = true;
					break;
				case SDLK_a:
					_camera->moveLeft = true;
					break;
				case SDLK_s:
					_camera->moveBack = true;
					break;
				case SDLK_d:
					_camera->moveRight = true;
					break;
				case SDLK_LALT:
					if (_camera->timeCounter >= 1.f) {
						_camera->enableMouse = !_camera->enableMouse;
						SDL_ShowCursor(_camera->enableMouse);
						_camera->timeCounter = 0.f;
					}
					break;
				default:
					break;
				}
				switch (event.type) {
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_LSHIFT)
						_camera->pressedShift = false;
					if (event.key.keysym.sym == SDLK_SPACE)
						_camera->moveUp = false;
					if (event.key.keysym.sym == SDLK_LCTRL)
						_camera->moveDown = false;
					if (event.key.keysym.sym == SDLK_w)
						_camera->moveForward = false;
					if (event.key.keysym.sym == SDLK_a)
						_camera->moveLeft = false;
					if (event.key.keysym.sym == SDLK_s)
						_camera->moveBack = false;
					if (event.key.keysym.sym == SDLK_d)
						_camera->moveRight = false;
					break;
				case SDL_QUIT:
					quit = true;
					break;
				default:
					break;
				}
			}
		}

		_camera->update(deltaTime);

		_normalShader->useProgram();
		_normalShader->setValue(0, _camera->getViewMatrix());
		_normalShader->setValue(1, _camera->getProjectionMatrix());
		_normalShader->setValue(2, 0);
		_cannonTexture->bind(0);

		_renderer->render(_models[0], _normalShader);

		SDL_GL_SwapWindow(_window->getWindow());
	}
	return 0;
}

void Engine::_init() {
	_initSDL();
	_initGL();

	_meshLoader = std::make_unique<MeshLoader>();
	_models.push_back(_meshLoader->getTriangleMesh());
	_models.push_back(_meshLoader->loadMesh("assets/models/duck.fbx"));

	_normalShader = std::make_shared<ShaderProgram>("Test Pass");
	_normalShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/normal.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/normal.frag")
		.finalize();

	_textureLoader = std::make_unique<TextureLoader>();
	_cannonTexture = _textureLoader->loadTexture("assets/textures/rainbow.png");

	_camera = std::make_shared<Camera>();
}

void Engine::_initSDL() {
	_window = std::make_unique<Window>("Memengine");
}

void Engine::_initGL() {
	_renderer = std::make_unique<Renderer>(_window->getWindow());
}