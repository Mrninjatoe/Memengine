#pragma once
#include <glad/glad.h>
#include <stdio.h>
#include "window.hpp"
#include "model.hpp"
#include "shaderprogram.hpp"
#include "shadowcaster.hpp"
#include "framebuffer.hpp"
#include "camera.hpp"

class Renderer {
public:
	Renderer(SDL_Window* window);
	virtual ~Renderer();
	void render(const std::vector<std::shared_ptr<Model>>& models, const std::shared_ptr<ShaderProgram>& shader);
	void renderFullScreenQuad(const std::shared_ptr<Model>& quad);
	void renderShadows(const std::vector<std::shared_ptr<Model>>& models, const std::shared_ptr<ShaderProgram>& shader,
		const std::shared_ptr<Framebuffer>& fbo, const std::shared_ptr<Shadowcaster>& caster);
	void showGuizmo(const std::shared_ptr<Camera>& camera);
	SDL_GLContext& getContext() { return _context; }
private:
	SDL_GLContext _context;

	static void APIENTRY _openglCallbackFunction(
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
		//fprintf(stderr, "%s\n", message);

		//printf("%s, ", message);
		if (severity == GL_DEBUG_SEVERITY_HIGH) {
			fprintf(stderr, "Aborting...\n");
			abort();
		}
	}
};