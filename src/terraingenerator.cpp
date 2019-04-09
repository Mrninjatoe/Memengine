#include "terraingenerator.hpp"

TerrainGenerator::TerrainGenerator(){
	_numPatches = 96;
	_tileSize = 1.f;
}

TerrainGenerator::~TerrainGenerator(){

}

void TerrainGenerator::initialize() {
	auto fRand = [](float fMin, float fMax) {
		float f = (float)rand() / RAND_MAX;
		return fMin + f * (fMax - fMin);
	};

	Quad quad;
	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;

	quad.vertices[0].color = glm::vec3(1, 0, 0);
	quad.vertices[1].color = glm::vec3(0, 1, 0);
	quad.vertices[2].color = glm::vec3(0, 0, 1);
	quad.vertices[3].color = glm::vec3(1, 1, 1);

	quad.vertices[0].normal = glm::vec3(0, 1, 0);
	quad.vertices[1].normal = glm::vec3(0, 1, 0);
	quad.vertices[2].normal = glm::vec3(0, 1, 0);
	quad.vertices[3].normal = glm::vec3(0, 1, 0);

	quad.vertices[0].pos = glm::vec3(-0.5, 0, -0.5);
	quad.vertices[1].pos = glm::vec3(-0.5, 0, 0.5);
	quad.vertices[2].pos = glm::vec3(0.5, 0, 0.5);
	quad.vertices[3].pos = glm::vec3(0.5, 0, -0.5);

	quad.vertices[0].tangent = glm::vec3(1, 0, 0);
	quad.vertices[1].tangent = glm::vec3(1, 0, 0);
	quad.vertices[2].tangent = glm::vec3(1, 0, 0);
	quad.vertices[3].tangent = glm::vec3(1, 0, 0);

	quad.vertices[0].uv = glm::vec2(0, 0);
	quad.vertices[1].uv = glm::vec2(0, 1);
	quad.vertices[2].uv = glm::vec2(1, 1);
	quad.vertices[3].uv = glm::vec2(1, 0);

	indices = {0, 1, 3, 1, 2, 3};

	for (int i = 0; i < 4; i++)
		vertices.push_back(quad.vertices[i]);

	std::vector<glm::vec3> posOffsets;
	glm::vec2 uvOffsets = glm::vec2(0, 0);
	float offsetIncrement = 1.f / (float)(_numPatches * _numPatches);
	float axisMax = _numPatches * 0.5f;
	for (float x = 0; x < _numPatches; x++)
		for (float z = 0; z < _numPatches; z++)
			posOffsets.push_back(glm::vec3(x * offsetIncrement, 0, z * offsetIncrement));

	_mesh = std::make_shared<Mesh>(vertices, indices);
	_mesh->setupInstancedBuffer(posOffsets);
}