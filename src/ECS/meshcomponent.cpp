#include "ECS/meshcomponent.hpp"

using namespace ECS::Component;

MeshComponent::~MeshComponent() {
	
}

const std::string MeshComponent::type() const {
	return "MeshComponent";
}