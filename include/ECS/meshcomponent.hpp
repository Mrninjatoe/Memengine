#pragma once
#include "world.hpp"
#include "model.hpp"

namespace ECS::Component {
	struct MeshComponent final : public ECS::World::SubComponent<MeshComponent, ComponentBits::Mesh> {
		~MeshComponent() final;
		const std::string type() const;
		std::shared_ptr<Model> model;
	};
}