#pragma once
#include <string>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include "mesh.hpp"
#include "model.hpp"

class MeshLoader {
public:
	MeshLoader();
	virtual ~MeshLoader();
	std::shared_ptr<Model> loadMesh(const std::string& filePath);
	std::shared_ptr<Model> getTriangleMesh();
private:
	void _processNode(aiNode* node, const aiScene* scene, const std::string& path);
	Mesh _processMesh(aiMesh* mesh, const aiScene* scene);

	std::map<const std::string, std::shared_ptr<Model>> _models;
};