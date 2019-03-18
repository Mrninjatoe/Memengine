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

class Engine {
	public:
		Engine();
		virtual ~Engine() {
			printf("Starting deletion of Members in Engine!\n");
			_camera.reset();
			_textureLoader.reset();
			_meshLoader.reset();
			_renderer.reset();
			_window.reset();
			_models.clear();
			printf("Done deleting member variable for the Engine!\n");
		}

		static Engine* getInstance() {
			if (!_instance)
				_instance = new Engine();

			return _instance;
		}
		int run();
		Window* getWindow() { return _window.get(); }
		Renderer* getRenderer() { return _renderer.get(); }
		MeshLoader* getMeshLoader() { return _meshLoader.get(); }
		TextureLoader* getTextureLoader() { return _textureLoader.get(); }
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

		std::shared_ptr<ShaderProgram> _shadowShader;
		std::shared_ptr<ShaderProgram> _cShadowDebugShader;
		std::shared_ptr<ShaderProgram> _csFrustumDebugShader;
		std::shared_ptr<ShaderProgram> _normalShader;
		std::shared_ptr<ShaderProgram> _renderFBOShader;
		std::shared_ptr<ShaderProgram> _cubemapShader;
		std::shared_ptr<ShaderProgram> _gaussianFilterShader;
		std::shared_ptr<Framebuffer> _shadowFramebuffer;
		std::shared_ptr<Framebuffer> _geometryFramebuffer;
		std::shared_ptr<Texture> _cubeMapTexture;

		std::shared_ptr<Model> _quad;
		std::shared_ptr<Model> _cubeMapModel;
		std::vector<std::shared_ptr<Model>> _models;
		std::vector<std::shared_ptr<Model>> _instancedModels;
		std::vector<std::shared_ptr<Pointlight>> _lights;

		std::shared_ptr<Camera> _camera;
		std::shared_ptr<Shadowcaster> _shadowCaster;
};