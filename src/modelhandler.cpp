#include "modelhandler.hpp"
#include "engine.hpp"

ModelHandler::ModelHandler() {
	
}

ModelHandler::~ModelHandler() {
	
}

std::shared_ptr<Model> ModelHandler::createModel(const std::string& meshPath) {
	_models[meshPath].push_back(Engine::getInstance()->getMeshLoader()->loadMesh(meshPath));

	return _models[meshPath].back();
}

std::map<std::string, std::vector<std::shared_ptr<Model>>> ModelHandler::getAllModels() {

	return _models;
}

// Since each model has shared ptrs to the mesh, updating one model should update them all.
// That's the reason for tuple.second[0]...
// Should make it only update the model matrices that has changed.
void ModelHandler::updateInstancedModels() {
	for (auto tuple : _models) {
		if (tuple.second.size() == 1) // No need for instancing if it's only one.
			continue;

		std::vector<glm::mat4> newMatrices;

		for (auto model : tuple.second)
			newMatrices.push_back(model->getModelMatrix());

		if (!tuple.second[0]->isInstanced())
			tuple.second[0]->makeInstanceable(newMatrices);
		else
			tuple.second[0]->updateInstanceInfo(newMatrices);
	}
}