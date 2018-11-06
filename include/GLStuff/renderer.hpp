#pragma once
#include <glad/glad.h>
#include <stdio.h>
#include "window.hpp"

class Renderer {
public:
	Renderer(SDL_Window* window);
	virtual ~Renderer();


	static void APIENTRY openglCallbackFunction(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam
	) {
		(void)source; (void)type; (void)id;
		(void)severity; (void)length; (void)userParam;
		fprintf(stderr, "%s\n", message);

		printf("%s, ");
		if (severity == GL_DEBUG_SEVERITY_HIGH) {
			fprintf(stderr, "Aborting...\n");
			//abort();
		}
	}
private:
	SDL_GLContext _context;
};