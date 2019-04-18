#pragma once
#include "world.hpp"

namespace ECS::Component {
	struct TransformComponent final : public ECS::World::SubComponent<TransformComponent, ComponentBits::Transform> {
		~TransformComponent() final;
		const std::string type() const;
		float temp = 1.0f;
	};
}