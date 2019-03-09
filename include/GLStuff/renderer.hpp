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
	void renderShadows(const std::vector<std::shared_ptr<Model>>& models, const std::shared_ptr<ShaderProgram>& shader,
		const std::shared_ptr<Framebuffer>& fbo, const std::shared_ptr<Shadowcaster>& caster);
	void gaussianFilter(const std::shared_ptr<Texture>& toBlur, const std::shared_ptr<Model>& quad, const std::shared_ptr<ShaderProgram>& gaussianShader);
	void renderFullScreenQuad(const std::shared_ptr<Model>& quad);
	void renderCubemap(const std::shared_ptr<Model>& cubemapModel);

	void showGuizmo(const std::shared_ptr<Camera>& camera);
	SDL_GLContext& getContext() { return _context; }
	std::shared_ptr<Texture> getPingPongTexture() { return _pingPongBuffers[1]->getTexture(0); }
private:
	SDL_GLContext _context;
	std::shared_ptr<Framebuffer> _pingPongBuffers[2];
	std::shared_ptr<Texture> _pingPongTextures[2];

	void _setUpPingPong() {
		_pingPongBuffers[0] = std::make_shared<Framebuffer>("Ping Pong 0");
		_pingPongBuffers[1] = std::make_shared<Framebuffer>("Ping Pong 1");
		_pingPongTextures[0] = std::make_shared<Texture>();
		_pingPongTextures[0]->intializeVSMTex(glm::ivec2(1024));
		_pingPongTextures[1] = std::make_shared<Texture>();
		_pingPongTextures[1]->intializeVSMTex(glm::ivec2(1024));
		for (unsigned int i = 0; i < 2; i++) {
			_pingPongBuffers[i]->bind();
			_pingPongBuffers[i]->attachTexture(0, _pingPongTextures[i])
				.finalize();
		}
	}

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
		if (id == 4 || id == 8 || id == 11 || id == 20 || id == 36 || id == 37 || id == 48 || id == 1282 || id == 131169 || id == 131185 || id == 131218 || id == 131204)
			return;

		fprintf(stderr, "%s\n", message);
		//printf("%s, ", message);
		if (severity == GL_DEBUG_SEVERITY_HIGH) {
			fprintf(stderr, "Aborting...\n");
			abort();
		}
	}
};