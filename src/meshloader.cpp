#include "meshloader.hpp"

MeshLoader::MeshLoader() {
	
}

MeshLoader::~MeshLoader() {
	
}

std::shared_ptr<Model> MeshLoader::loadMesh(const std::string& filePath){
	auto meshAlreadyExists = _models.count(filePath);
	if (meshAlreadyExists)
		return _models[filePath];
	
	const aiScene* scene = aiImportFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		printf("ERROR::ASSIMP::%s", aiGetErrorString());

	_models[filePath] = std::make_shared<Model>();
	_processNode(scene->mRootNode, scene, filePath);

	return _models[filePath];
} 

// TO-DO: Fix this class and load in meshes.
void MeshLoader::_processNode(aiNode* node, const aiScene* scene, const std::string& path) {
	printf("NumMeshes in aiScene: %u\n", scene->mNumMeshes);
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_models[path]->addMesh(_processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
		_processNode(node->mChildren[i], scene, path);
}


Mesh MeshLoader::_processMesh(aiMesh* mesh, const aiScene* scene) {
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

		if (mesh->mTextureCoords[0]) {
			glm::vec2 texCoordData;
			texCoordData.x = mesh->mTextureCoords[0][i].x;
			texCoordData.x = mesh->mTextureCoords[0][i].y;
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

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// load textures.
	}
	
	Mesh newMesh = Mesh(vertices, indices);
		
	return newMesh;
}

std::shared_ptr<Model> MeshLoader::getTriangleMesh() {
	auto triangleAlreadyExists = _models.count("triangle");
	if (triangleAlreadyExists)
		return _models["triangle"];
	
	_models["triangle"] = std::make_shared<Model>();

	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;

	vertices = {
		Mesh::Vertex{glm::vec3(-0.5f,-0.5f,0), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec2(0,0)},
		Mesh::Vertex{glm::vec3(-0.5f,0.5f,0), glm::vec3(0,0,1), glm::vec3(0,1,0), glm::vec2(0,1)},
		Mesh::Vertex{glm::vec3(0.5f,0.5f,0), glm::vec3(0,0,1), glm::vec3(0,0,1), glm::vec2(1,1)},
		Mesh::Vertex{glm::vec3(0.5f,-0.5f,0), glm::vec3(0,0,1), glm::vec3(0,0,1), glm::vec2(1,0)}
	};
	indices = {
		0, 1, 3,
		1, 2, 3
	};

	_models["triangle"]->addMesh(Mesh(vertices, indices));

	return _models["triangle"];
}