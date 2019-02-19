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
	if (meshAlreadyExists)
		return _models[fileName];
	
	const aiScene* scene = aiImportFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		printf("ERROR::ASSIMP::%s", aiGetErrorString());

	_models[fileName] = std::make_shared<Model>();
	printf("Number of textures: %u\n", scene->mNumTextures);
	_processNode(scene->mRootNode, scene, fileName);

	return _models[fileName];
} 

// TO-DO: Fix this class and load in meshes.
void MeshLoader::_processNode(aiNode* node, const aiScene* scene, const std::string& fileName) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_models[fileName]->addMesh(_processMesh(mesh, scene, fileName));
		printf("Mesh:%u\n", i);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
		_processNode(node->mChildren[i], scene, fileName);
}


std::shared_ptr<Mesh> MeshLoader::_processMesh(aiMesh* mesh, const aiScene* scene, const std::string& fileName) {
	printf("Processing a mesh!\n");
	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;

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

		meshData.x = mesh->mTangents[i].x;
		meshData.y = mesh->mTangents[i].y;
		meshData.z = mesh->mTangents[i].z;
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

	auto newMesh = std::make_shared<Mesh>(vertices, indices);
	
	if (mesh->mMaterialIndex >= 0) {
		printf("Screaming Pepega\n");
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			printf("yas\n");
			newMesh->addTexture(_loadMaterialTexture(scene, material, aiTextureType_DIFFUSE, fileName));
		}
		if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
			printf("normals pepega\n");
			newMesh->addTexture(_loadMaterialTexture(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_NORMALS, fileName));
		}
		//std::shared_ptr<Texture> specularMap = _loadMaterialTexture(scene, material, aiTextureType_SPECULAR, path);
	}

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

//std::shared_ptr<Texture> MeshLoader::_loadMaterialTexture(const aiScene* scene, const aiMaterial* mat, aiTextureType type, const std::string& filePath) {
//	// FIX THIS GARBAGE >:(
//	aiString path;
//	mat->GetTexture(type, 0, &path);
//
//	if (path.data[0] == '*') {
//		int index = atoi(path.data + 1);
//		std::string mapKey = filePath + path.data[1];
//		aiTexture* tex = scene->mTextures[index];
//		printf("%i\n", atoi(path.data + 1));
//		printf("%i\n", tex->mWidth);
//		printf("%s\n", tex->achFormatHint);
//		printf("%p\n", tex->pcData);
//		printf("\n");
//
//		if (tex->mHeight != 0) {
//			return Engine::getInstance()->getTextureLoader()->loadEmbeddedTexture(tex->mWidth, tex->mHeight, (void*)tex->pcData, mapKey);
//		}
//		else
//			return Engine::getInstance()->getTextureLoader()->loadEmbeddedTexture(tex->achFormatHint, tex->mWidth, (void*)tex->pcData, mapKey);
//	}
//	else {
//		std::string fullPath = path.C_Str();
//		printf("Full filepath for textures: %s\n", fullPath.c_str());
//		return Engine::getInstance()->getTextureLoader()->loadTexture(fullPath);
//	}
//}

std::shared_ptr<Model> MeshLoader::getTriangleMesh() {
	auto triangleAlreadyExists = _models.count("quad");
	if (triangleAlreadyExists)
		return _models["quad"];
	
	_models["quad"] = std::make_shared<Model>();

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

	_models["quad"]->addMesh(std::make_shared<Mesh>(vertices, indices));

	return _models["quad"];
}