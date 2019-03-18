#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>
#include <vector>
#include "camera.hpp"

class Shadowcaster {
public:
	Shadowcaster();
	Shadowcaster(const int& numOfSplits, const int& resolution);
	virtual ~Shadowcaster();
	void update(float dt);
	void createCascadeSplits(const std::shared_ptr<Camera>& playerCamera);
	inline glm::vec3 getPos() { return _pos; }
	inline std::vector<glm::mat4>& getViewProjMatrices() { return _cascadedViewProjs; }
	inline std::vector<float>& getCascadedSplits() { return _cascadedSplits; }
	void makeStatic(const bool& condition);
	inline bool& isStatic() { return _static; }
	inline int getResolution() { return _resolution; }

	float lambda = 1.f;
	float minDistance = 0.f;
	float maxDistance = 0.016f;
	int numCascadeSplits;
private:
	float* _intermediateCascadeSplits;
	bool _static = false;
	glm::vec3 _pos;
	std::vector<float> _cascadedSplits;
	std::vector<glm::mat4> _cascadedViewProjs;
	float _timeCounter;
	int _resolution; // Always *2 for width/height.

	void _initialize();
};