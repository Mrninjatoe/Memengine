#include "GLStuff/renderer.hpp"
#include "engine.hpp"
#include "imgui/imGuizmo/ImGuizmo.h"
#include "imgui/imgui.h"
#include <glm/gtx/matrix_decompose.hpp>

Renderer::Renderer(SDL_Window* window){
	// Request an OpenGL 4.4 Core context
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	// Also request a depth buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG
	);

	_context = SDL_GL_CreateContext(window);
	if (_context == NULL)
		printf("SDL failed to create a GL_Context!\n");
	else
		printf("SDL successfully created a GL_Context!\n");

	printf("OpenGL loaded\n");
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));

	// Enable the debug callback
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(_openglCallbackFunction, nullptr);
	glDebugMessageControl(
		GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true
	);

	SDL_GL_SetSwapInterval(1);
	
	// TODO: Actually use them properly lamog.
	glEnable(GL_DEPTH_TEST); // Default
	glDepthFunc(GL_LESS); // Default
	glEnable(GL_CULL_FACE); // Default
	glCullFace(GL_BACK); // Default
	glDepthMask(GL_TRUE);// Default
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Default
}

Renderer::~Renderer() {
	printf("~Renderer()\n");
	for (unsigned int i = 0; i < 2; i++) {
		_pingPongTextures[i].reset();
		_pingPongBuffers[i].reset();
	}
	SDL_GL_DeleteContext(Engine::getInstance()->getWindow());
}

void Renderer::render(const std::shared_ptr<ShaderProgram> shader) {
	// Write to depth, do depth test, cull back.
	const auto& sizes = Engine::getInstance()->getWindow()->getSize();
	glViewport(0, 0, sizes.x, sizes.y);
	glClearColor(1,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_CULL_FACE);
	shader->setValue(20, 0); // diff
	shader->setValue(21, 1); // normal
	shader->setValue(22, 2); // height

	for (auto mapContainer : Engine::getInstance()->getModelHandler()->getAllModels()) {
		for (auto model : mapContainer.second) {
			auto instanceCount = (unsigned int)mapContainer.second.size();
			shader->setValue(2, model->getModelMatrix());
			shader->setValue(4, instanceCount);
			for (auto mesh : model->getMeshes()) {
				mesh->getTextures()[(int)Mesh::TextureLocation::diffuse]->bind(0);
				mesh->getTextures()[(int)Mesh::TextureLocation::normalMap]->bind(1);
				mesh->getTextures()[(int)Mesh::TextureLocation::heightMap]->bind(2);
				shader->setValue(13, mesh->hasParallax());
				glBindVertexArray(mesh->getVAO());
				glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mesh->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)instanceCount);
			}
			if (instanceCount > 1) // Ugly exit for now
				break;
		}
	}

	glBindVertexArray(0);
	//glEnable(GL_CULL_FACE);
}

void Renderer::renderShadows(const std::shared_ptr<ShaderProgram> shader, const std::shared_ptr<Framebuffer> fbo,
	const std::shared_ptr<Shadowcaster> caster) {
	// Write to depth, do depth test, cull front. Have depth clamp.
	auto size = fbo->getTexture(0)->getSize();
	glViewport(0, 0, size.x, size.y);
	glClearColor(1, 1, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_DEPTH_CLAMP);
	//glCullFace(GL_FRONT);
	int shaderPosLSMatrix = 5;
	for (auto lsMatrix : caster->getViewProjMatrices())
		shader->setValue(shaderPosLSMatrix++, lsMatrix);

	shader->setValue(20, 0); // diffuse texture to remove alpha for shadows.
	for (auto tuple : Engine::getInstance()->getModelHandler()->getAllModels()) {
		for (auto model : tuple.second) {
			auto instanceCount = (unsigned int)tuple.second.size();
			shader->setValue(0, model->getModelMatrix());
			shader->setValue(1, instanceCount);
			for (auto mesh : model->getMeshes()) {
				mesh->getTextures()[(int)Mesh::TextureLocation::diffuse]->bind(0);
				glBindVertexArray(mesh->getVAO());
				glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mesh->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)instanceCount);
			}
			if (instanceCount > 1) // Ugly exit for instanced rendering.
				break;
		}
	}
	glBindVertexArray(0);
	//glCullFace(GL_BACK);
	glDisable(GL_DEPTH_CLAMP);
}

void Renderer::gaussianFilter(const std::shared_ptr<Texture> toBlur, const std::shared_ptr<Model> quad, 
	const std::shared_ptr<ShaderProgram> gaussianShader) {
	// use viewport from shadow pass.
	gaussianShader->useProgram();
	gaussianShader->setValue(20, 0); // Texture location color_0.
	auto quadMesh = quad->getMeshes()[0]; // Have to fix this :pepeLaugh:
	unsigned int amount = 1 * 2;
	glm::vec2 direction = {0,0};
	bool horizontal = true, firstIteration = true;
	gaussianShader->setValue(2, 0.5f);
	glDisable(GL_CULL_FACE);
	for (unsigned int i = 0; i < amount; i++) {
		horizontal ? direction = glm::vec2(1, 0) : direction = glm::vec2(0, 1);
		_pingPongBuffers[horizontal]->bind();
		gaussianShader->setValue(1, direction);
		firstIteration ? toBlur->bind(0, true) : _pingPongBuffers[!horizontal]->getTexture(0)->bind(0, true);
		glBindVertexArray(quadMesh->getVAO());
		glDrawElements(GL_TRIANGLES, (GLsizei)quadMesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
		horizontal = !horizontal;
		if (firstIteration)
			firstIteration = false;
	}
}

void Renderer::renderFullScreenQuad(const std::shared_ptr<Model> quad) {
	// Write to depth, depth test, cull nothing.
	const auto& sizes = Engine::getInstance()->getWindow()->getSize();
	glViewport(0, 0, sizes.x, sizes.y); // change viewport back to screen size.
	glClearColor(1,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//glDisable(GL_CULL_FACE); //Needs to add this if gaussian pass is off.
	
	glBindVertexArray(quad->getMeshes()[0]->getVAO());
	glDrawElements(GL_TRIANGLES, (GLsizei)quad->getMeshes()[0]->getIndices().size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Renderer::postProcessFXAA(const std::shared_ptr<Model> quad) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_TRUE);

	glBindVertexArray(quad->getMeshes()[0]->getVAO());
	glDrawElements(GL_TRIANGLES, (GLsizei)quad->getMeshes()[0]->getIndices().size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	// Depthfunc changes in cubemap rendering.
}

void Renderer::renderTerrain(const std::shared_ptr<Mesh> terrain, const int& instanceCount) {

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(terrain->getVAO());
	glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)terrain->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)instanceCount);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(0);
}

void Renderer::renderSkydome(const std::shared_ptr<Model> model) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	glBindVertexArray(model->getMeshes()[0]->getVAO());
	glDrawElements(GL_TRIANGLES, (GLsizei)model->getMeshes()[0]->getIndices().size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Renderer::showGuizmo(const std::shared_ptr<Camera> camera) {
	auto currentlySelected = camera->currentTarget.lock();
	if (currentlySelected == nullptr)
		return;

	static ImGuizmo::OPERATION currentOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE currentMode(ImGuizmo::WORLD);
	ImGui::Begin("Camera's Selected Entity");
	ImGui::Text("Z: Translate, X: Rotate, C: Scale.");
	if (camera->zPressed)
		currentOperation = ImGuizmo::TRANSLATE;
	else if (camera->xPressed)
		currentOperation = ImGuizmo::ROTATE;
	else if(camera->cPressed)
		currentOperation = ImGuizmo::SCALE;

	static bool useSnap = false;
	if (ImGui::IsKeyPressed('9'))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();

	glm::vec3* snap = nullptr;
	switch (currentOperation) {
	case ImGuizmo::TRANSLATE:
		static glm::vec3 snapTranslation = glm::vec3(0.1f);
		snap = &snapTranslation;
		ImGui::InputFloat3("Snap", glm::value_ptr(snapTranslation));
		break;
	case ImGuizmo::ROTATE:
		static glm::vec3 snapRotation = glm::vec3(0.1f);
		snap = &snapTranslation;
		ImGui::InputFloat3("Angle Snap", glm::value_ptr(snapTranslation));
		break;
	case ImGuizmo::SCALE:
		break;
	default:
		break;
	}
	ImGui::End();

	// ImGuizmo part
	glm::mat4 modelMatrix = currentlySelected->getModelMatrix();
	glm::mat4 deltaMatrix;

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(camera->getViewMatrix()), glm::value_ptr(camera->getProjectionMatrix()),
		currentOperation, currentMode, glm::value_ptr(modelMatrix), glm::value_ptr(deltaMatrix), 
		useSnap ? glm::value_ptr(*snap) : nullptr);

	glm::vec3 scale;
	glm::quat orientation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(deltaMatrix, scale, orientation, translation, skew, perspective);

	static bool didUse = false;
	if (ImGuizmo::IsUsing()) {
		didUse = false;
		switch (currentOperation)
		{
		case ImGuizmo::TRANSLATE:
			currentlySelected->applyPosition(translation);
			break;
		case ImGuizmo::ROTATE:
			currentlySelected->applyOrientation(orientation);
			break;
		case ImGuizmo::SCALE: {
			glm::vec3 tempScale = currentlySelected->getPrevScaling() + (scale - glm::vec3(1.f));
			currentlySelected->applyTempScaling(tempScale);
			break;
		}
		case ImGuizmo::BOUNDS:
			break;
		default:
			break;
		}
	}
}


void Renderer::enableGaussianForVSM(const std::shared_ptr<Shadowcaster> caster) {
	_pingPongBuffers[0] = std::make_shared<Framebuffer>("Ping Pong 0");
	_pingPongBuffers[1] = std::make_shared<Framebuffer>("Ping Pong 1");
	_pingPongTextures[0] = std::make_shared<Texture>();
	_pingPongTextures[0]->intializeVSMTex(glm::ivec2(caster->getResolution()), caster->numCascadeSplits);
	_pingPongTextures[1] = std::make_shared<Texture>();
	_pingPongTextures[1]->intializeVSMTex(glm::ivec2(caster->getResolution()), caster->numCascadeSplits);
	for (unsigned int i = 0; i < 2; i++) {
		_pingPongBuffers[i]->bind();
		_pingPongBuffers[i]->attachTexture(0, _pingPongTextures[i])
			.finalize();
	}
}