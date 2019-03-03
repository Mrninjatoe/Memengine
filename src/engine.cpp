#include "engine.hpp"
#include <SDL2/SDL_events.h>
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"
#include "imGui/imGuizmo/ImGuizmo.h"

// Thanks to Dear imgui and imGuizmo :pray:
// Actually looks cool now Pog.

// TO-DO: Fix blending between cascades - Not done yet.
// Fix world picking actually using proper hitboxes.
// Ambient occlusion (?)

Engine* Engine::_instance;
Engine::Engine() {

}

int Engine::run() {
	_init();
	SDL_Event event;

	// Move imgui 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.WantCaptureKeyboard = true;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(_window->getWindow(), _renderer->getContext());
	ImGui_ImplOpenGL3_Init("#version 440 core");
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	float deltaTime = 0.0;

	bool quit = false;
	bool debugCascades = false;
	while (!quit) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = ((NOW - LAST) * 1000 / (float)SDL_GetPerformanceFrequency()) * 0.001f;

		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
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
			case SDLK_z:
				_camera->zPressed = true;
				_camera->xPressed = false;
				_camera->cPressed = false;
				break;
			case SDLK_x:
				_camera->zPressed = false;
				_camera->xPressed = true;
				_camera->cPressed = false;
				break;
			case SDLK_c:
				_camera->zPressed = false;
				_camera->xPressed = false;
				_camera->cPressed = true;
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
			case SDLK_r:
				_camera->untargeted = true;
				_camera->currentTarget.reset();
				break;
			default:
				break;
			}
			switch (event.button.button) {
			case SDL_BUTTON_LEFT:
				if (!_camera->leftClick)
					_camera->leftClick = true;
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
			case SDL_MOUSEBUTTONUP: {
				_camera->leftClick = false;
				if (auto target = _camera->currentTarget.lock()) {
					target->setPrevScaling(target->getScaling());
				}
				break;
			}
			case SDL_QUIT:
				quit = true;
				break;
			default:
				break;
			}
		}

		{	// ImGui memes.
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(_window->getWindow());
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();

			bool cascadeIsStatic = _shadowCaster->isStatic();
			ImGui::Begin("Graphic Settings");
			ImGui::Text("Cascading shadow map attributes");
			ImGui::Checkbox("Enable Cascade Debug", &debugCascades);
			ImGui::Checkbox("Static Cascade Splits", &cascadeIsStatic);
			if (cascadeIsStatic == false) {
				ImGui::SliderFloat("Lambda", &_shadowCaster->lambda, 0.f, 1.0f);
				ImGui::SliderFloat("Min Distance", &_shadowCaster->minDistance, 0.f, 1.0f);
				ImGui::SliderFloat("Max Distance", &_shadowCaster->maxDistance, 0.f, 1.0f);
			}
			ImGui::End();
			_shadowCaster->makeStatic(cascadeIsStatic);
		}

		_camera->update(deltaTime);

		{ // Shadow memes
			_shadowCaster->update(deltaTime);
			_shadowCaster->createCascadeSplits(_camera, 1024); // 1024*1024 = texture size. Fix for shadowcaster.

			_shadowShader->useProgram();
			_shadowFramebuffer->bind();
			_renderer->renderShadows(_models, _shadowShader, _shadowFramebuffer, _shadowCaster);
		}

		{	// Geometry pass
			_normalShader->useProgram();
			_normalShader->setValue(0, _camera->getViewMatrix());
			_normalShader->setValue(1, _camera->getProjectionMatrix());
			_geometryFramebuffer->bind();
			_renderer->render(_models, _normalShader);
		}

		{ // GBuffer textures and shadow map and other stuff.
			_renderFBOShader->useProgram();
			_renderFBOShader->setValue(18, _shadowCaster->getPos());
			_renderFBOShader->setValue(19, _camera->pos);
			_renderFBOShader->setValue(20, 0);
			_renderFBOShader->setValue(21, 1);
			_renderFBOShader->setValue(22, 2);
			_renderFBOShader->setValue(23, 3);
			_geometryFramebuffer->getTexture(0)->bind(0);
			_geometryFramebuffer->getTexture(1)->bind(1);
			_geometryFramebuffer->getTexture(2)->bind(2);
			_geometryFramebuffer->getTexture(3)->bind(3);

			_renderFBOShader->setValue(24, 4);
			_shadowFramebuffer->getTexture(0)->bind(4, true);
			_renderFBOShader->setValue(25, 4);
			int mxShaderPos = 26;
			for (int i = 0; i < 4; i++) {
				_renderFBOShader->setValue(mxShaderPos++, _shadowCaster->getViewProjMatrix(i));
			}
			_renderFBOShader->setValue(30, _shadowCaster->getCascadedSplits());
			_renderFBOShader->setValue(31, _window->getSize());
			_renderFBOShader->setValue(32, _camera->getViewMatrix());
			_renderFBOShader->setValue(33, debugCascades);

			_renderFBOShader->setValue(34, (int)_lights.size());
			int shaderPos = 35;
			for (int i = 0; i < _lights.size(); i++) {
				auto light = _lights[i];
				_renderFBOShader->setValue(shaderPos++, light->pos);
				_renderFBOShader->setValue(shaderPos++, light->color);
			}

			_renderer->renderFullScreenQuad(_quad);
		}

		{	// Render guizmo for camera's currently selected entity. (model for now)
			_renderer->showGuizmo(_camera);
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(_window->getWindow());
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	delete _instance;
	return 0;
}

void Engine::_init() {
	_initSDL();
	_initGL();

	_meshLoader = std::make_unique<MeshLoader>();
	
	_shadowShader = std::make_shared<ShaderProgram>("Shadow Pass");
	_shadowShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/shadowpass.vert")
		.attachShader(ShaderProgram::ShaderType::GeometryShader, "assets/shaders/shadowpass.geom")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/shadowpass.frag")
		.finalize();

	_cShadowDebugShader = std::make_shared<ShaderProgram>("Cascading Shadowmap Debug");
	_cShadowDebugShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/shadowdebug.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/shadowdebug.frag")
		.finalize();

	_csFrustumDebugShader = std::make_shared<ShaderProgram>("Cascading Shadowmap Frustum Debug");
	_csFrustumDebugShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/frustumdebug.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/frustumdebug.frag")
		.finalize();

	_normalShader = std::make_shared<ShaderProgram>("Test Pass");
	_normalShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/normal.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/normal.frag")
		.finalize();

	_renderFBOShader = std::make_shared<ShaderProgram>("Render FBO pass");
	_renderFBOShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/renderbufferpass.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/renderbufferpass.frag")
		.finalize();
	
	_geometryFramebuffer = std::make_shared<Framebuffer>();
	_geometryFramebuffer->attachTexture(0, _window->getSize(), Texture::TextureFormat::RGBA32f) // Pos 0
		.attachTexture(1, _window->getSize(), Texture::TextureFormat::RGBA32f) // Normal 1
		.attachTexture(2, _window->getSize(), Texture::TextureFormat::RGBA32f) // Color 2
		.attachTexture(3, _window->getSize(), Texture::TextureFormat::Depth32f) // Depth 3
		.finalize();

	_shadowFramebuffer = std::make_shared<Framebuffer>("Cascading Shadowmap FBO");
	_shadowFramebuffer->attachTexture(0, glm::ivec2(1024), Texture::TextureFormat::Depth32f, true) // Shadow depths
		.finalize();

	_textureLoader = std::make_unique<TextureLoader>();
	_camera = std::make_shared<Camera>();
	_camera->pos = glm::vec3(0,0,-10);
	_shadowCaster = std::make_shared<Shadowcaster>();
	//_shadowCaster->createCascadeSplits(_camera, 1024);
	
	_initWorld();

	/*_lightsUBO = std::make_shared<Uniformbuffer>();
	_lightsUBO->setData(_lights);*/
}

void Engine::_initSDL() {
	_window = std::make_unique<Window>("Memengine");
}

void Engine::_initGL() {
	_renderer = std::make_unique<Renderer>(_window->getWindow());
}
void Engine::_initWorld() {
	_models.push_back(_meshLoader->loadMesh("sheagle_box.fbx"));
	_models[0]->setPosition(glm::vec3(0, 1, 0));
	_models.push_back(_meshLoader->loadMesh("flipys_box.fbx"));
	_models[1]->setPosition(glm::vec3(10, 1, 0));
	_models.push_back(_meshLoader->loadMesh("box.fbx"));
	_models[2]->setPosition(glm::vec3(0, 1, 10));
	_models.push_back(_meshLoader->loadMesh("kuri_box.fbx"));
	_models[3]->setPosition(glm::vec3(5, 1, 5));
	_models.push_back(_meshLoader->loadMesh("wrench_box.fbx"));
	_models[4]->setPosition(glm::vec3(-10, 1, 0));
	_models.push_back(_meshLoader->loadMesh("cannon_box.fbx"));
	_models[5]->setPosition(glm::vec3(0, 1, -10));
	_models.push_back(_meshLoader->loadMesh("duck.fbx"));
	_models[6]->setPosition(glm::vec3(-15, 5, 15)).setScaling(glm::vec3(5));
	_models.push_back(_meshLoader->loadMesh("plane.fbx"));
	_models[7]->setPosition(glm::vec3(0, 0, 0))
		.setScaling(glm::vec3(10000, 1, 10000));
	_models.push_back(_meshLoader->loadMesh("sheagle_box.fbx"));
	_models[8]->setPosition(glm::vec3(8, 1, 8));
	_models.push_back(_meshLoader->loadMesh("isak_tecken.fbx"));
	_models[9]->setPosition(glm::vec3(25, 2, 25)).setScaling(glm::vec3(10));


	//for (int x = -10; x < 10; x++) {
	//	for (int z = -10; z < 10; z++) {
	//		auto tecken = _meshLoader->loadMesh("isak_tecken.fbx");
	//		tecken->setPosition(glm::vec3(x * 100, 0, z * 100)).setScaling(glm::vec3(10));
	//		_models.push_back(tecken);
	//	}
	//}

	_quad = _meshLoader->getFullscreenQuad();
	_cubeDebug = _meshLoader->getCubeMesh();

	_lights.push_back(std::make_shared<Pointlight>(glm::vec3(0, 10, -10), glm::vec3(1, 1, 1)));
}