#pragma once
#include <memory>
#include "window.hpp"
#include "GLStuff/renderer.hpp"

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
	private:
		void _init();
		void _initSDL();
		void _initGL();
		static Engine* _instance;
		
		std::unique_ptr<Window> _window;
		std::unique_ptr<Renderer> _renderer;
};