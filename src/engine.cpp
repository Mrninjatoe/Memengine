#include "engine.hpp"
#include <SDL2/SDL_events.h>
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>

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
	float rotationTemp = 0.f;
	while (!quit) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = ((NOW - LAST) * 1000 / (float)SDL_GetPerformanceFrequency()) * 0.001f;

		while (SDL_PollEvent(&event)) {
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
			case SDLK_LCTRL:
				_camera->moveDown = true;
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
		// Fix memory leaks in application.

		_camera->update(deltaTime);
		
		for (auto model : _models) {
			model->setRotation(rotationTemp, glm::vec3(0, 1, 0));
		}
		
		_normalShader->useProgram();
		_normalShader->setValue(0, _camera->pos);
		_normalShader->setValue(1, _camera->getViewMatrix());
		_normalShader->setValue(2, _camera->getProjectionMatrix());
		_geometryFramebuffer->bind();
		_renderer->render(_models, _normalShader);

		_renderFBOShader->useProgram();
		_renderFBOShader->setValue(20, 0);
		_geometryFramebuffer->getTexture(0)->bind(0);
		_renderer->renderFBOContent(_quad);

		rotationTemp += 1.f * deltaTime;
		
		SDL_GL_SwapWindow(_window->getWindow());
	}

	delete _instance;
	return 0;
}

void Engine::_init() {
	_initSDL();
	_initGL();

	_meshLoader = std::make_unique<MeshLoader>();
	
	_normalShader = std::make_shared<ShaderProgram>("Test Pass");
	_normalShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/normal.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/normal.frag")
		.finalize();

	_renderFBOShader = std::make_shared<ShaderProgram>("Render FBO pass");
	_renderFBOShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/renderbufferpass.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/renderbufferpass.frag")
		.finalize();
	
	_geometryFramebuffer = std::make_shared<Framebuffer>();
	_geometryFramebuffer->attachTexture(0, _window->getSize(), Texture::TextureFormat::RGBA32f);

	_textureLoader = std::make_unique<TextureLoader>();
	_camera = std::make_shared<Camera>();
	
	_initWorld();
}

void Engine::_initSDL() {
	_window = std::make_unique<Window>("Memengine");
}

void Engine::_initGL() {
	_renderer = std::make_unique<Renderer>(_window->getWindow());
}
void Engine::_initWorld() {

	_models.push_back(_meshLoader->loadMesh("sheagle_box.fbx"));
	_models[0]->setPosition(glm::vec3(5, 0, 5));
	_models.push_back(_meshLoader->loadMesh("flipys_box.fbx"));
	_models[1]->setPosition(glm::vec3(5, 2, 5));
	_models.push_back(_meshLoader->loadMesh("box.fbx"));
	_models[2]->setPosition(glm::vec3(5, 4, 5));
	_models.push_back(_meshLoader->loadMesh("kuri_box.fbx"));
	_models[3]->setPosition(glm::vec3(5, 6, 5));

	_quad = _meshLoader->getTriangleMesh();
}