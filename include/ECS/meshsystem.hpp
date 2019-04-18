#pragma once
#include "world.hpp"

namespace ECS::System {
	class MeshSystem final : public ECS::System::SubSystem<MeshSystem> {
		~MeshSystem() final;
		void update() final;
	};
}