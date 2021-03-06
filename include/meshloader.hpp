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
	std::shared_ptr<Model> loadMesh(const std::string& fileName);
	std::shared_ptr<Model> getFullscreenQuad();
	std::shared_ptr<Model> getLineCubeMesh();
private:
	void _processNode(aiNode* node, const aiScene* scene, const std::string& fileName);
	std::shared_ptr<Mesh> _processMesh(aiMesh* mesh, const aiScene* scene, const std::string& fileName);
	std::shared_ptr<Texture> _loadMaterialTexture(const aiScene* scene, const aiMaterial* mat, aiTextureType type, const std::string& fileName);

	std::map<const std::string, Model> _models;
};