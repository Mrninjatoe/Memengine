#pragma once
#include <string>
#include <memory.h>
#include <vector>
#include <unordered_map>

namespace ECS::Component {
#define BIT(x) (1 << x)
	enum class ComponentBits : uint64_t {
		Transform = BIT(0),
		Mesh = BIT(1)
	};
#undef BIT

	inline ComponentBits& operator |=(ComponentBits& a, const ComponentBits& b) { *reinterpret_cast<uint64_t*>(&a) |= static_cast<uint64_t>(b); return a; }
	inline ComponentBits& operator &=(ComponentBits& a, const ComponentBits& b) { *reinterpret_cast<uint64_t*>(&a) &= static_cast<uint64_t>(b); return a; }
	inline ComponentBits  operator | (const ComponentBits a, const ComponentBits b) { return static_cast<ComponentBits>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b)); }
	inline ComponentBits  operator & (const ComponentBits a, const ComponentBits b) { return static_cast<ComponentBits>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b)); }
	inline ComponentBits  operator ~ (const ComponentBits a) { return static_cast<ComponentBits>(~static_cast<uint64_t>(a)); }
}

// Have to forward declare subcomponent and that it's a template.
namespace ECS::World {
	template <typename T, ECS::Component::ComponentBits bit>
	struct SubComponent;
}

// Have to forward delcare new components.
namespace ECS::Component {
	struct TransformComponent;
	struct MeshComponent;
}

// Forward declare all systems.
namespace ECS::System {
	class MeshSystem;
}

namespace ECS::System {
	class BaseSystem {
	public:
		virtual ~BaseSystem() = 0;
		virtual void update() = 0;
	};
	inline BaseSystem::~BaseSystem() {
		printf("BaseSystem\n");
	}

	template <typename T>
	class SubSystem : public BaseSystem {
	public:
		virtual ~SubSystem() = 0;
	};

	template class SubSystem<MeshSystem>;
	// Template destructors defined in world.cpp
}

namespace ECS::World {
	typedef size_t EntityID;

	struct BaseComponent {
		EntityID entityID;
		virtual ~BaseComponent() = 0;
		virtual const std::string type() const = 0;
	};
	inline BaseComponent::~BaseComponent() { 
		printf("~BaseComponent\n"); 
	}

	class BaseComponentHandler {
	public:
		virtual ~BaseComponentHandler() = 0;
		virtual const std::vector<std::shared_ptr<BaseComponent>>& getActiveComponents() = 0;
		virtual std::shared_ptr<BaseComponent> getComponent(EntityID id) = 0;
		virtual std::shared_ptr<BaseComponent> addComponent(EntityID id) = 0;
		virtual void removeComponent(EntityID id) = 0;
	};
	inline BaseComponentHandler::~BaseComponentHandler() { 
		printf("~BaseComponentHandler()\n"); 
	}

	template <typename T>
	class ComponentHandler : public BaseComponentHandler {
	public:
		std::unordered_map<EntityID, size_t> map;
		std::vector<std::shared_ptr<BaseComponent>> components;

		const std::vector<std::shared_ptr<BaseComponent>>& getActiveComponents() final {
			return components;
		};

		std::shared_ptr<BaseComponent> getComponent(EntityID inID) final {
			return components[map[inID]];
		}

		std::shared_ptr<BaseComponent> addComponent(EntityID inID) final {
			auto comp = new T();
			comp->entityID = inID;
			components.push_back(std::shared_ptr<BaseComponent>(comp));
			map[inID] = components.size() - 1;
			return components.back();
		}

		void removeComponent(EntityID inID) final {
			if (const size_t pos = map[inID]; pos != components.size() - 1) {
				map[components.back()->entityID] = pos;
				std::swap(components[pos], components.back());
			}
			components.pop_back();
			map.erase(inID);
		}
	};

	template <typename T, ECS::Component::ComponentBits bit>
	struct SubComponent : public BaseComponent {
		static BaseComponentHandler* componentHandler;
		static constexpr ECS::Component::ComponentBits bits = bit;

		virtual ~SubComponent() = 0;
		//virtual const std::string type() const = 0;
	};

	// ComponentHandler/Component declarations.
	BaseComponentHandler* SubComponent<ECS::Component::TransformComponent, ECS::Component::ComponentBits::Transform>::componentHandler;
	BaseComponentHandler* SubComponent<ECS::Component::MeshComponent, ECS::Component::ComponentBits::Mesh>::componentHandler;
	
	template struct SubComponent<ECS::Component::TransformComponent, ECS::Component::ComponentBits::Transform>;
	template struct SubComponent<ECS::Component::MeshComponent, ECS::Component::ComponentBits::Mesh>;

	struct Entity {
		EntityID id;
		ECS::Component::ComponentBits activeComponents;
		std::string name;
		~Entity();

		template <typename T>
		inline bool hasComponent() const { return (activeComponents & T::bits) == T::bits; }
		
		inline bool hasComponents(ECS::Component::ComponentBits inBits) const { 
			return (activeComponents & inBits) == inBits; 
		}
		
		inline size_t componentCount() {
			// Counts the number of bits set in activeComponents
			// Literally magic code I have no idea how this works.
			uint64_t i = static_cast<uint64_t>(activeComponents);
			i = i - ((i >> 1) & 0x5555555555555555UL);
			i = (i & 0x3333333333333333UL) + ((i >> 2) & 0x3333333333333333UL);
			return (int)((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
		}

		template <typename T>
		inline std::shared_ptr<T> getComponent() const {
			if (hasComponents(T::bits))
				return std::static_pointer_cast<T>(T::componentHandler->getComponent(id));
			return std::shared_ptr<T>();
		}

		template <typename T>
		inline std::shared_ptr<T> addComponent() {
			if (hasComponents(T::bits))
				return std::static_pointer_cast<T>(T::componentHandler->getComponent(id));
			activeComponents |= T::bits;
			return std::static_pointer_cast<T>(T::componentHandler->addComponent(id));
		}

		template <typename T>
		inline void removeComponent() {
			if (!hasComponents(T::bits))
				return;
			activeComponents &= ~T::bits;
			 T::componentHandler->removeComponent(id);
		}
	};
}

class World {
public:
	World();
	~World();
	void update(float delta);
	std::shared_ptr<ECS::World::Entity> addEntity(const std::string& name);
	void removeEntity(ECS::World::EntityID id);
private:
	std::unordered_map<ECS::World::EntityID, size_t> _map;
	std::vector<std::shared_ptr<ECS::World::Entity>> _entities;

	static ECS::World::EntityID _idCounter;
	void _generateHandlers();
};