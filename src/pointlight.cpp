#include "pointlight.hpp"

Pointlight::Pointlight() : pos(0,0,0), color(1,1,1){
	
}

Pointlight::Pointlight(const glm::vec3& inPos, const glm::vec3& inColor) : pos(inPos), color(inColor) {
	
}

Pointlight::~Pointlight() {
	
}

