#include "ECS/transformcomponent.hpp"

using namespace ECS::Component;

TransformComponent::~TransformComponent() {
	printf("Death consumes me\n");
}

const std::string TransformComponent::type() const {
	return "TransformComponent";
}