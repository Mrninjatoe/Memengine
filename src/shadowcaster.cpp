#include "shadowcaster.hpp"
#include "engine.hpp"
#include <limits>

// This class provides shadows for large scenes. 
// Information and most of the code was accquired from Johan and his blog regarding cascading shadow maps.
// Webpage link: https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
// Determining split distances follows the practical split scheme by Nvidia
// Webpage link: https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html


// Still a bit broken, too big difference between orthographic boxes.
// CURRENT PROBLEM: SPLIT 1,2, AND 3 ARE TOO LARGE.
// HAS TO DO WITH SOMETHING. 


Shadowcaster::Shadowcaster() : _pos(0, 1.f, -1.f), _timeCounter(0), 
numCascadeSplits(4), _resolution(1024){
	_initialize();
}

Shadowcaster::Shadowcaster(const int& numSplits, const int& resolution) : _pos(0, 1.f, -1.f), _timeCounter(0.f), 
numCascadeSplits(numSplits), _resolution(resolution) {
	_initialize();
}

Shadowcaster::~Shadowcaster(){
	delete _intermediateCascadeSplits;
	_cascadedSplits.clear();
	_cascadedViewProjs.clear();
}

void Shadowcaster::update(float dt) {
	float radius = 1.f;
	float camX = sin(_timeCounter) * radius;
	float camZ = cos(_timeCounter) * radius;

	_pos.x = camX;
	_pos.z = camZ;

	_timeCounter += 1 * dt;
}

void Shadowcaster::createCascadeSplits(const std::shared_ptr<Camera>& playerCamera) {
	// 4 = max splits, should make a constant.
	{
		float nearClip = playerCamera->zNear; // curr camera
		float farClip = playerCamera->zFar; // curr camera

		if (_static) {
			_intermediateCascadeSplits[0] = 0.001f;
			_intermediateCascadeSplits[1] = 0.007f;
			_intermediateCascadeSplits[2] = 0.020f;
			_intermediateCascadeSplits[3] = 0.070f;
		}
		else {
			float clipRange = farClip - nearClip;
			float minZ = nearClip + minDistance * clipRange;
			float maxZ = farClip + maxDistance * clipRange;
			float range = maxZ - minZ;
			float ratio = maxZ / minZ;
			// Enable for pratical split scheme.
			for (unsigned int i = 0; i < (unsigned int)numCascadeSplits; i++) {
				float p = (i + 1) / static_cast<float>(numCascadeSplits);
				float log = minZ * std::pow(ratio, p);
				float uniform = minZ + range * p;
				float d = lambda * (log - uniform) + uniform;
				_intermediateCascadeSplits[i] = (d - nearClip) / clipRange;
			}
		}

		for (unsigned int cascadeIndex = 0; cascadeIndex < (unsigned int)numCascadeSplits; cascadeIndex++) {
			float prevSplitDist = cascadeIndex == 0 ? _intermediateCascadeSplits[0] : _intermediateCascadeSplits[cascadeIndex - 1];
			float splitDist = _intermediateCascadeSplits[cascadeIndex];
		
			// Frustrum corners in view space..
			glm::vec3 frustumCorners[8]{
				glm::vec3(-1.0f, 1.0f, -1.0f),
				glm::vec3(1.0f, 1.0f, -1.0f),
				glm::vec3(1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, -1.0f, 1.0f),
				glm::vec3(-1.0f, -1.0f, 1.0f)
			};
		
			// Fixing the frustrum corners to worldspace.
			glm::mat4 invViewProj = glm::inverse(playerCamera->getProjectionMatrix() * playerCamera->getViewMatrix());
			for (unsigned int i = 0; i < 8; i++) {
				glm::vec4 invPoint = invViewProj * glm::vec4(frustumCorners[i], 1.f);
				frustumCorners[i] = glm::vec3(invPoint / invPoint.w);
			}
		
			for (unsigned int i = 0; i < 4; i++) {
				glm::vec3 cornerRay = frustumCorners[i + 4] - frustumCorners[i];
				glm::vec3 nearCornerRay = cornerRay * prevSplitDist;
				glm::vec3 farCornerRay = cornerRay * splitDist;
				frustumCorners[i + 4] = frustumCorners[i] + farCornerRay; // AABB Corners far
				frustumCorners[i] = frustumCorners[i] + nearCornerRay; // AABB Corners near
			}
		
			glm::vec3 frustumCenter = glm::vec3(0.f);
			for (unsigned int i = 0; i < 8; i++)
				frustumCenter += frustumCorners[i];
			frustumCenter /= 8.f;
		
			glm::vec3 min = glm::vec3(std::numeric_limits<float>::infinity());
			glm::vec3 max = glm::vec3(-std::numeric_limits<float>::infinity());
			for (int i = 0; i < 8; i++) {
				min = glm::min(min, frustumCorners[i]);
				max = glm::max(max, frustumCorners[i]);
			}
		
			float radius = std::ceil(glm::distance(min, max) / 2.f);
			glm::vec3 maxExtents = glm::vec3(radius, radius, radius);	
			glm::vec3 minExtents = -maxExtents;
		
			glm::mat4 lightView = glm::mat4(1.f);
			glm::vec3 shadowEye = frustumCenter - (-glm::normalize(_pos)); 
			lightView = glm::lookAt(shadowEye, frustumCenter, glm::vec3(0, 1.f, 0));
			glm::vec3 cascadeExtents = maxExtents - minExtents;
			glm::mat4 lightProj = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 
				minExtents.z, maxExtents.z);
		
			{ // Shadow matrix to make CSM stable. Otherwise we'll have shadow shimmering.
				glm::mat4 shadowMatrix = lightProj * lightView;
				glm::vec4 shadowOrigin = glm::vec4(0, 0, 0, 1.f);
				shadowOrigin = shadowMatrix * shadowOrigin;
				shadowOrigin = shadowOrigin * ((float)_resolution - 1) / 2.f; // shadowMapSize
		
				glm::vec4 roundedOrigin = glm::round(shadowOrigin);
				glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
				roundOffset = roundOffset * (2.f / ((float)_resolution - 1));
				roundOffset.z = 0.f;
				roundOffset.w = 0.f;
				glm::mat4 shadowProj = lightProj;
				shadowProj[3] += roundOffset;
				lightProj = shadowProj;
			}
		
			const float clipDist = farClip - nearClip;
			_cascadedSplits[cascadeIndex] = (nearClip + splitDist * clipDist) * -1; // convert to eye space z value.
			_cascadedViewProjs[cascadeIndex] = lightProj * lightView;
		}
	}
}

void Shadowcaster::makeStatic(const bool& condition) {
	_static = condition;
}

void Shadowcaster::_initialize() {
	_cascadedSplits.resize(numCascadeSplits);
	_intermediateCascadeSplits = new float[numCascadeSplits];
	_cascadedViewProjs.resize(numCascadeSplits);
}