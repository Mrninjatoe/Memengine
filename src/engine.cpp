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

// TO-DO: Fix instanced rendering :)

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
	io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
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

	// Temp graphics settings.
	float heightScale = 0.1f;
	float minLayers = 8.f;
	float maxLayers = 32.f;
	//VCSM
	float lowVSMValue = 0.2f;
	float highVSMValue = 1.0f;
	float variancePower = 0.00001f;
	bool enablePicking = true;
	//FXAA
	float fxaaSpanMax = 8.f;
	float fxaaReduceMin = 1.f / 128.f;
	float fxaaReduceMul = 1.f / 8.f;

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
			{ // VSM
				ImGui::Text("VSM Attributes");
				ImGui::SliderFloat("lowVSMValue", &lowVSMValue, 0.f, 1.0f);
				ImGui::SliderFloat("highVSMValue", &highVSMValue, 0.f, 1.0f);
				ImGui::SliderFloat("variancePower", &variancePower, 0.00001f, 0.0001f, "%.5f");
				ImGui::Text("Parallax Mapping Attributes");
				ImGui::SliderFloat("HeightScale", &heightScale, 0.f, 1.f);
				ImGui::SliderFloat("MinLayers", &minLayers, 1.f, 100.f);
				ImGui::SliderFloat("MaxLayers", &maxLayers, 1.f, 100.f);
				ImGui::Checkbox("Stop Sun Rotation", &_shadowCaster->getRotation());
			}
			{ // FXAA
				ImGui::BeginChild("FXAA Settings");
				ImGui::Text("FXAA Attributes");
				ImGui::SliderFloat("fxaaSpanMax", &fxaaSpanMax, 1.f, 16.f);
				ImGui::SliderFloat("fxaaReduceMin", &fxaaReduceMin, 1.f / 256.f, 1.f / 128.f);
				ImGui::SliderFloat("fxaaReduceMul", &fxaaReduceMul, 1.f / 8.f, 1.f / 1.f);
				ImGui::EndChild();
			}
			{ // World picking stuff
				ImGui::BeginChild("World Settings");
				ImGui::Text("Generic Entity Control");
				ImGui::Checkbox("Enable World Picking", &enablePicking);
				ImGui::EndChild();
			}
			ImGui::End();
			_camera->enablePicking = enablePicking;
			_shadowCaster->makeStatic(cascadeIsStatic);
		}

		_camera->update(deltaTime);
		_shadowCaster->update(deltaTime);
		_shadowCaster->createCascadeSplits(_camera); // Cascading shadow maps split per frame...
		_modelHandler->updateInstancedModels();

		{	// 1.0 Geometry pass
			_normalShader->useProgram();
			_normalShader->setValue(0, _camera->getViewMatrix());
			_normalShader->setValue(1, _camera->getProjectionMatrix());
			_normalShader->setValue(3, _camera->pos);
			_normalShader->setValue(10, heightScale);
			_normalShader->setValue(11, minLayers);
			_normalShader->setValue(12, maxLayers);
			
			_geometryFramebuffer->bind();
			_renderer->render(_models, _normalShader);
		}

		{ // 2.0 Shadow memes
			_shadowShader->useProgram();
			
			_shadowFramebuffer->bind();
			_renderer->renderShadows(_models, _shadowShader, _shadowFramebuffer, _shadowCaster);
		}

		{ // 2.5 Use gaussian filter to blur shadows for lighting pass.
			_renderer->gaussianFilter(_shadowFramebuffer->getTexture(0), _quad, _gaussianFilterShader);
		}

		{ // 3.0 GBuffer textures, shadow map and other stuff.
			_renderFBOShader->useProgram();
			_renderFBOShader->setValue(15, variancePower);
			_renderFBOShader->setValue(16, lowVSMValue);
			_renderFBOShader->setValue(17, highVSMValue);
			_renderFBOShader->setValue(18, _shadowCaster->getPos());
			_renderFBOShader->setValue(19, _camera->pos);
			_renderFBOShader->setValue(20, 0);
			_renderFBOShader->setValue(21, 1);
			_renderFBOShader->setValue(22, 2);
			_renderFBOShader->setValue(23, 3);
			_renderFBOShader->setValue(24, 4);
			_geometryFramebuffer->getTexture(Framebuffer::TextureAttachment::positions)->bind(0);
			_geometryFramebuffer->getTexture(Framebuffer::TextureAttachment::normals)->bind(1);
			_geometryFramebuffer->getTexture(Framebuffer::TextureAttachment::colors)->bind(2);
			_geometryFramebuffer->getTexture(Framebuffer::TextureAttachment::depths)->bind(3);
			//_shadowFramebuffer->getTexture(0)->bind(4, true);
			_renderer->getPingPongTexture()->bind(4, true);

			_renderFBOShader->setValue(25, _window->getSize());
			_renderFBOShader->setValue(26, _camera->getViewMatrix());
			_renderFBOShader->setValue(27, debugCascades);
			
			_renderFBOShader->setValue(28, _shadowCaster->numCascadeSplits);
			int splitShaderPos = 29;
			for (auto split : _shadowCaster->getCascadedSplits())
				_renderFBOShader->setValue(splitShaderPos++, split);
			
			int mxShaderPos = 37;
			for (auto shadowViewProjMX : _shadowCaster->getViewProjMatrices())
				_renderFBOShader->setValue(mxShaderPos++, shadowViewProjMX);

			_renderFBOShader->setValue(45, (int)_lights.size());
			int lightShaderPos = 46;
			for (int i = 0; i < _lights.size(); i++) {
				auto light = _lights[i];
				_renderFBOShader->setValue(lightShaderPos++, light->pos);
				_renderFBOShader->setValue(lightShaderPos++, light->color);
			}

			_lightingFramebuffer->bind();
			_renderer->renderFullScreenQuad(_quad);
		}

		{	// 4.0 FXAA Post Processing
			auto sceneTexture = _lightingFramebuffer->getTexture(0);
			auto textureSizes = sceneTexture->getSize();
			_fxaaShader->useProgram();
			_fxaaShader->setValue(15, fxaaSpanMax);
			_fxaaShader->setValue(16, fxaaReduceMin);
			_fxaaShader->setValue(17, fxaaReduceMul);
			_fxaaShader->setValue(18, glm::vec2(1.f / textureSizes.x, 1.f / textureSizes.y));
			_fxaaShader->setValue(20, 0);
			_fxaaShader->setValue(21, 1);
			sceneTexture->bind(0);
			_geometryFramebuffer->getTexture(Framebuffer::TextureAttachment::depths)->bind(1);
			_renderer->postProcessFXAA(_quad);
		}

		{	// 5.0 Cube map memes
			_cubemapShader->useProgram();
			_cubemapShader->setValue(0, glm::mat4(glm::mat3(_camera->getViewMatrix())));
			_cubemapShader->setValue(1, _camera->getProjectionMatrix());
			_cubemapShader->setValue(20, 0);
			_cubeMapTexture->bindCubemap(0);

			_renderer->renderCubemap(_cubeMapModel);
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

	_textureLoader = std::make_unique<TextureLoader>();
	_meshLoader = std::make_unique<MeshLoader>();
	_modelHandler = std::make_unique<ModelHandler>();
	
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

	_cubemapShader = std::make_shared<ShaderProgram>("Cubemap Pass");
	_cubemapShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/cubemap.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/cubemap.frag")
		.finalize();

	_gaussianFilterShader = std::make_shared<ShaderProgram>("Gaussian Filter Shader");
	_gaussianFilterShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/filter-gaussian.vert")
		.attachShader(ShaderProgram::ShaderType::GeometryShader, "assets/shaders/filter-gaussian.geom")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/filter-gaussian.frag")
		.finalize();

	_fxaaShader = std::make_shared<ShaderProgram>("FXAA Shader");
	_fxaaShader->attachShader(ShaderProgram::ShaderType::VertexShader, "assets/shaders/fxaa.vert")
		.attachShader(ShaderProgram::ShaderType::FragmentShader, "assets/shaders/fxaa.frag")
		.finalize();
	
	_geometryFramebuffer = std::make_shared<Framebuffer>();
	_geometryFramebuffer->createTexture(0, _window->getSize(), Texture::TextureFormat::RGBA32f) // Pos 0
		.createTexture(1, _window->getSize(), Texture::TextureFormat::RGBA32f) // Normal 1
		.createTexture(2, _window->getSize(), Texture::TextureFormat::RGBA32f) // Color 2
		.createTexture(3, _window->getSize(), Texture::TextureFormat::Depth32f) // Depth 3
		.finalize();

	_shadowCaster = std::make_shared<Shadowcaster>(4, 2048);

	auto shadowMapTex = std::make_shared<Texture>();
	shadowMapTex->intializeVSMTex(glm::ivec2(_shadowCaster->getResolution()), _shadowCaster->numCascadeSplits);

	_shadowFramebuffer = std::make_shared<Framebuffer>("Cascading Shadowmap FBO");
	_shadowFramebuffer->attachTexture(0, shadowMapTex)
		.createTexture(1, glm::ivec2(_shadowCaster->getResolution()), Texture::TextureFormat::Depth32f, true).finalize();

	_lightingFramebuffer = std::make_shared<Framebuffer>();
	_lightingFramebuffer->createTexture(0, _window->getSize(), Texture::TextureFormat::RGBA32f)
		.finalize();

	_cubeMapTexture = _textureLoader->loadCubeMapTexture("skybox");

	_camera = std::make_shared<Camera>();
	_camera->pos = glm::vec3(0,0,-10);

	_renderer->enableGaussianForVSM(_shadowCaster);
	
	_initWorld();
}

void Engine::_initSDL() {
	_window = std::make_unique<Window>("Memengine");
}

void Engine::_initGL() {
	_renderer = std::make_unique<Renderer>(_window->getWindow());
}
void Engine::_initWorld() {
	_modelHandler->createModel("plane.fbx")->setPosition(glm::vec3(0)).setScaling(glm::vec3(2000, 1, 2000));
	_modelHandler->createModel("magicBox.obj")->setPosition(glm::vec3(0, 10, -5)).setScaling(glm::vec3(2));
	_modelHandler->createModel("treeVersion4.fbx")->setPosition(glm::vec3(5, 0, 5)).setScaling(glm::vec3(2));
	
	auto fRand = [](float fMin, float fMax) {
		float f = (float)rand() / RAND_MAX;
		return fMin + f * (fMax - fMin);
	};

	const std::string treePaths[3] = { "lowPolyTree4.fbx", "lowPolyTree5.fbx", "lowPolyTree6.fbx"};

	for (int y = -10; y < 10; y++) {
		for (int x = -10; x < 10; x++) {
			//_modelHandler->createModel("lowPolyTree.fbx")
			//	->setPosition(glm::vec3(x, 0, y) * fRand(-100, 100))
			//	.setScaling(glm::vec3(1) * fRand(5, 10));

			/*_modelHandler->createModel("lowPolyTree4.fbx")->setPosition(glm::vec3(x * fRand(-100, 100), 0, y * fRand(-100, 100)))
				.setScaling(glm::vec3(1) * fRand(5, 10));*/

			//_modelHandler->createModel("lowPolyTree6.fbx")->setPosition(glm::vec3(x * fRand(-100, 100), 0, y * fRand(-100, 100)))
			//	.setScaling(glm::vec3(1) * fRand(5, 10));

			_modelHandler->createModel(treePaths[int(fRand(0, 3))])->setPosition(glm::vec3(x * fRand(-100, 100), 0, y * fRand(-100, 100)))
				.setScaling(glm::vec3(1) * fRand(5, 10));
		}
	}

	_quad = _meshLoader->getFullscreenQuad();
	_cubeMapModel = _meshLoader->loadMesh("cubeMapBox.fbx");
	_cubeMapModel->setScaling(glm::vec3(500.f));

	_lights.push_back(std::make_shared<Pointlight>(glm::vec3(0, 10, -10), glm::vec3(1, 1, 1)));
}