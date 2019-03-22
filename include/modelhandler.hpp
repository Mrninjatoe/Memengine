#pragma once
#include <map>
#include <model.hpp>

class ModelHandler {
public:
	ModelHandler();
	virtual ~ModelHandler();
	std::shared_ptr<Model> createModel(const std::string& meshPath);
	std::map<std::string, std::vector<std::shared_ptr<Model>>> getAllModels();
	void updateInstancedModels();
private:
	std::map<std::string, std::vector<std::shared_ptr<Model>>> _models;
};