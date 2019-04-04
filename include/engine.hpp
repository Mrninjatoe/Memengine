#pragma once
#include <memory>
#include "window.hpp"
#include "GLStuff/renderer.hpp"
#include "shaderprogram.hpp"
#include "meshloader.hpp"
#include "textureloader.hpp"
#include "camera.hpp"
#include "GLStuff/framebuffer.hpp"
#include "pointlight.hpp"
#include "GLStuff/uniformbuffer.hpp"
#include "shadowcaster.hpp"
#include "modelhandler.hpp"

class Engine {
	public:
		Engine();
		virtual ~Engine() {
			printf("Starting deletion of Members in Engine!\n");
			_camera.reset();
			_textureLoader.reset();
			_meshLoader.reset();
			_shadowCaster.reset();
			for (auto light : _lights) {
				light.reset();
			}
			
			_renderer.reset();
			_window.reset();
			_models.clear();
			printf("Done deleting member variable for the Engine!\n");
		}

		static Engine*getInstance() {
			if (!_instance)
				_instance = new Engine();

			return _instance;
		}
		int run();
		inline Window* getWindow() { return _window.get(); }
		inline Renderer* getRenderer() { return _renderer.get(); }
		inline MeshLoader* getMeshLoader() { return _meshLoader.get(); }
		inline TextureLoader* getTextureLoader() { return _textureLoader.get(); }
		inline ModelHandler* getModelHandler() { return _modelHandler.get(); }
		std::vector<std::shared_ptr<Model>>& getWorldObjects() { return _models; }
	private:
		void _init();
		void _initSDL();
		void _initGL();
		void _initWorld();
		static Engine* _instance;
		
		std::unique_ptr<Window> _window;
		std::unique_ptr<Renderer> _renderer;
		std::unique_ptr<TextureLoader> _textureLoader;
		std::unique_ptr<MeshLoader> _meshLoader;
		std::unique_ptr<ModelHandler> _modelHandler;

		std::shared_ptr<ShaderProgram> _shadowShader;
		std::shared_ptr<ShaderProgram> _cShadowDebugShader;
		std::shared_ptr<ShaderProgram> _csFrustumDebugShader;
		std::shared_ptr<ShaderProgram> _normalShader;
		std::shared_ptr<ShaderProgram> _renderFBOShader;
		std::shared_ptr<ShaderProgram> _cubemapShader;
		std::shared_ptr<ShaderProgram> _gaussianFilterShader;
		std::shared_ptr<ShaderProgram> _fxaaShader;
		std::shared_ptr<ShaderProgram> _atmosphericScatteringShader;
		std::shared_ptr<Framebuffer> _shadowFramebuffer;
		std::shared_ptr<Framebuffer> _geometryFramebuffer;
		std::shared_ptr<Framebuffer> _lightingFramebuffer;
		std::shared_ptr<Texture> _cubeMapTexture;

		std::shared_ptr<Model> _quad;
		std::shared_ptr<Model> _cubeMapModel;
		std::shared_ptr<Model> _skydome;
		std::vector<std::shared_ptr<Model>> _models;
		std::vector<glm::mat4> _tempMatrices;

		std::shared_ptr<Camera> _camera;
		std::shared_ptr<Shadowcaster> _shadowCaster;
		std::vector<std::shared_ptr<Pointlight>> _lights;
};