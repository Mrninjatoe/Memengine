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
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glDisable(GL_CULL_FACE);
}

Renderer::~Renderer() {
	printf("~Renderer()\n");
	SDL_GL_DeleteContext(Engine::getInstance()->getWindow());
}

void Renderer::render(const std::vector<std::shared_ptr<Model>>& models, const std::shared_ptr<ShaderProgram>& shader) {
	const auto& sizes = Engine::getInstance()->getWindow()->getSize();
	glViewport(0, 0, sizes.x, sizes.y);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	for (auto model : models) {
		shader->setValue(2, model->getModelMatrix());
		shader->setValue(20, 0);
		shader->setValue(21, 1);
		for (auto mesh : model->getMeshes()) {
			mesh->getTextures()[0]->bind(0);
			mesh->getTextures()[1]->bind(1);
			glBindVertexArray(mesh->getVAO());
			glDrawElements(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderFullScreenQuad(const std::shared_ptr<Model>& quad) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(quad->getMeshes()[0]->getVAO());
	glDrawElements(GL_TRIANGLES, quad->getMeshes()[0]->getIndices().size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Renderer::renderShadows(const std::vector<std::shared_ptr<Model>>& models, const std::shared_ptr<ShaderProgram>& shader, const std::shared_ptr<Framebuffer>& fbo,
	const std::shared_ptr<Shadowcaster>& caster) {
	auto size = fbo->getTexture(0)->getSize();
	glViewport(0, 0, size.x, size.y);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glCullFace(GL_FRONT);
	glClear(GL_DEPTH_BUFFER_BIT);
	int shaderPos = 5;
	for (int i = 0; i < 4; i++) {
		shader->setValue(shaderPos++, caster->getViewProjMatrix(i));
	}
	for (auto model : models) {
		shader->setValue(0, model->getModelMatrix());
		for (auto mesh : model->getMeshes()) {
			glBindVertexArray(mesh->getVAO());
			glDrawElements(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);
		}
	}	
	glCullFace(GL_BACK);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_CLAMP);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::showGuizmo(const std::shared_ptr<Camera>& camera) {
	auto currentlySelected = camera->currentTarget.lock();
	if (currentlySelected == nullptr)
		return;

	static ImGuizmo::OPERATION currentOperation(ImGuizmo::SCALE);
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
	printf("%f, %f, %f,%f\n", orientation.x, orientation.y, orientation.z, orientation.w);

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
			currentlySelected->setScaling(tempScale);
			break;
		}
		case ImGuizmo::BOUNDS:
			break;
		default:
			break;
		}
	}
}
