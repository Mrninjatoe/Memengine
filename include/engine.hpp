#pragma once
#include <memory>
#include "window.hpp"
#include "GLStuff/renderer.hpp"
#include "shaderprogram.hpp"
#include "meshloader.hpp"
#include "textureloader.hpp"

class Engine {
	public:
		Engine();
		~Engine() {
			delete _instance;
		}

		static Engine* getInstance() {
			if (!_instance)
				_instance = new Engine();

			return _instance;
		}
		int run();
		Window* getScreen() { return _window.get(); }
		Renderer* getRenderer() { return _renderer.get(); }
		MeshLoader* getMeshLoader() { return _meshLoader.get(); }
	private:
		void _init();
		void _initSDL();
		void _initGL();
		static Engine* _instance;
		
		std::unique_ptr<Window> _window;
		std::unique_ptr<Renderer> _renderer;
		std::unique_ptr<MeshLoader> _meshLoader;

		std::shared_ptr<ShaderProgram> _normalShader;
};