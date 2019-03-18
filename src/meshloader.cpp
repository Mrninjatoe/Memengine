#include "meshloader.hpp"
#include "engine.hpp"
#include <stdio.h>

// Fix deconstructor error for mesh...

MeshLoader::MeshLoader() {
	
}

MeshLoader::~MeshLoader() {
	printf("~MeshLoader()\n");
	_models.clear();
}

std::shared_ptr<Model> MeshLoader::loadMesh(const std::string& fileName){
	std::string filePath = "assets/models/" + fileName;
	auto meshAlreadyExists = _models.count(fileName);
	if (meshAlreadyExists) {
		printf("Mesh already exists!\n");
		return std::make_shared<Model>(_models[fileName]);
	}
	
	const aiScene* scene = aiImportFile(filePath.c_str(), aiProcess_Triangulate | 
		aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		printf("ERROR::ASSIMP::%s", aiGetErrorString());

	_models[fileName] = Model();
	_processNode(scene->mRootNode, scene, fileName);

	return std::make_shared<Model>(_models[fileName]);
} 

// TO-DO: Fix this class and load in meshes.
void MeshLoader::_processNode(aiNode* node, const aiScene* scene, const std::string& fileName) {
	printf("mesh\n");
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_models[fileName].addMesh(_processMesh(mesh, scene, fileName));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
		_processNode(node->mChildren[i], scene, fileName);
}


std::shared_ptr<Mesh> MeshLoader::_processMesh(aiMesh* mesh, const aiScene* scene, const std::string& fileName) {
	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;
	bool hasTangents = false;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Mesh::Vertex vertex;
		glm::vec3 meshData;

		meshData.x = mesh->mVertices[i].x;
		meshData.y = mesh->mVertices[i].y;
		meshData.z = mesh->mVertices[i].z;
		vertex.pos = meshData;
		
		meshData.x = mesh->mNormals[i].x;
		meshData.y = mesh->mNormals[i].y;
		meshData.z = mesh->mNormals[i].z;
		vertex.normal = meshData;

		if (mesh->mTangents != NULL) {
			meshData.x = mesh->mTangents[i].x;
			meshData.y = mesh->mTangents[i].y;
			meshData.z = mesh->mTangents[i].z;
			hasTangents = true;
		}
		else{
			meshData.x = 0.f;
			meshData.y = 0.f;
			meshData.z = 0.f;
		}
		vertex.tangent = meshData;

		if (mesh->mTextureCoords[0]) {
			glm::vec2 texCoordData;
			texCoordData.x = mesh->mTextureCoords[0][i].x;
			texCoordData.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = texCoordData;
		}
		else
			vertex.uv = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	bool hasParallax = false;
	auto newMesh = std::make_shared<Mesh>(vertices, indices);
	newMesh->setHasTangents(hasTangents);
	
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			newMesh->addTexture(_loadMaterialTexture(scene, material, aiTextureType_DIFFUSE, fileName));
		else
			newMesh->addTexture(Engine::getInstance()->getTextureLoader()->loadTexture("error_textures/error.png"));
		// Due to assimp having a problem differentiating between height/normal/disp. 
		// Height maps will always be normal maps from now on for obj and NORMALS for fbx.
		auto isFBX = fileName.find(".fbx");
		if (isFBX != std::string::npos) {
			if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
				newMesh->addTexture(_loadMaterialTexture(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_NORMALS, fileName));
			else
				newMesh->addTexture(Engine::getInstance()->getTextureLoader()->loadTexture("error_textures/error_normal.png"));
		}
		else { // actually is displacement map... Same thing I guess.
			if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
				newMesh->addTexture(_loadMaterialTexture(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_HEIGHT, fileName));
			else
				newMesh->addTexture(Engine::getInstance()->getTextureLoader()->loadTexture("error_textures/error_normal.png"));
		}
		if (material->GetTextureCount(aiTextureType_DISPLACEMENT) > 0) {
			newMesh->addTexture(_loadMaterialTexture(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DISPLACEMENT, fileName));
			hasParallax = true;
		}
		else
			newMesh->addTexture(Engine::getInstance()->getTextureLoader()->loadTexture("error_textures/error_displacement.png"));
	}
	newMesh->setHasParallax(hasParallax);

	return newMesh;
}

std::shared_ptr<Texture> MeshLoader::_loadMaterialTexture(const aiScene* scene, const aiMaterial* mat, aiTextureType type, const std::string& fileName) {
	std::string fullPath;
	aiString path;
	mat->GetTexture(type, 0, &path);
	std::string name = fileName.substr(0, fileName.find(".")) + '/';
	fullPath = name + path.C_Str();
	//printf("Full filepath for textures: %s\n", name.c_str());
	return Engine::getInstance()->getTextureLoader()->loadTexture(fullPath);
}

std::shared_ptr<Model> MeshLoader::getFullscreenQuad() {
	auto triangleAlreadyExists = _models.count("quad");
	if (triangleAlreadyExists)
		return std::make_shared<Model>(_models["quad"]);
	
	_models["quad"] = Model();

	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;

	vertices = {
		Mesh::Vertex{glm::vec3(-1.f,-1.f,0), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec3(0), glm::vec2(0,0)},
		Mesh::Vertex{glm::vec3(-1.f,1.f,0),  glm::vec3(0,0,1), glm::vec3(0,1,0), glm::vec3(0), glm::vec2(0,1)},
		Mesh::Vertex{glm::vec3(1.f,1.f,0),   glm::vec3(0,0,1), glm::vec3(0,0,1), glm::vec3(0), glm::vec2(1,1)},
		Mesh::Vertex{glm::vec3(1.f,-1.f,0),  glm::vec3(0,0,1), glm::vec3(0,0,1), glm::vec3(0), glm::vec2(1,0)}
	};
	indices = {
		0, 1, 3,
		1, 2, 3
	};

	_models["quad"].addMesh(std::make_shared<Mesh>(vertices, indices));

	return std::make_shared<Model>(_models["quad"]);
}

std::shared_ptr<Model> MeshLoader::getLineCubeMesh() {
	auto cubeAlreadyExists = _models.count("linecube");
	if (cubeAlreadyExists)
		return std::make_shared<Model>(_models["linecube"]);

	_models["linecube"] = Model();

	std::vector<Mesh::Vertex> vertices{
			Mesh::Vertex{ { -0.5, -0.5, -0.5 },{ 0,0,0 },{ 0,0,0 }, {0,0,0}, { 0,0 } }, // 0
			Mesh::Vertex{ { -0.5, 0.5, -0.5 },{ 0,0,0 },{ 0,0,0 },{0,0,0}, { 0,0 } },
			Mesh::Vertex{ { 0.5, 0.5, -0.5 },{ 0,0,0 },{ 0,0,0 },{0,0,0}, { 0,0 } },
			Mesh::Vertex{ { 0.5, -0.5, -0.5 },{ 0,0,0 },{ 0,0,0 },{0,0,0}, { 0,0 } },
			Mesh::Vertex{ { 0.5, -0.5, 0.5 },{ 0,0,0 },{ 0,0,0 },{0,0,0}, { 0,0 } },
			Mesh::Vertex{ { 0.5, 0.5, 0.5 },{ 0,0,0 },{ 0,0,0 },{0,0,0},{ 0,0 } },
			Mesh::Vertex{ { -0.5, 0.5, 0.5 },{ 0,0,0 },{ 0,0,0 },{0,0,0},{ 0,0 } },
			Mesh::Vertex{ { -0.5, -0.5, 0.5 },{ 0,0,0 },{ 0,0,0 },{0,0,0},{ 0,0 } },
	};

	std::vector<unsigned int> indices{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		3, 4,
		4, 5,
		5, 2,
		5, 6,
		6, 7,
		7, 4,
		7, 0,
		6, 1
	};

	_models["linecube"].addMesh(std::make_shared<Mesh>(vertices, indices));

	return std::make_shared<Model>(_models["linecube"]);
}