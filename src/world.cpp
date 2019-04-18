#include <cstdio>
#include "world.hpp"
#include "ECS/transformcomponent.hpp"
#include "ECS/meshcomponent.hpp"

using namespace ECS::World; // QoL otherwise I'd scream.
using namespace ECS::Component;
using namespace ECS::System;

//template <typename T, ECS::Component::ComponentBits bit>
//BaseComponentHandler* SubComponent<T, bit>::componentHandler;

template <typename T, ECS::Component::ComponentBits bit>
SubComponent<T, bit>::~SubComponent() {
	printf("Do I even work Pepega\n");
}

template <typename T>
SubSystem<T>::~SubSystem() {
	
}

EntityID World::_idCounter = 0;

Entity::~Entity() {
	printf("~Entity()\n");
}

World::World() {
	_generateHandlers();
}

World::~World() {
	printf("~World()\n");
	_entities.clear();
	_map.clear();
	// Have to add each deletion of handler here manually.
	delete SubComponent<ECS::Component::TransformComponent, ECS::Component::ComponentBits::Transform>::componentHandler;
	delete SubComponent<ECS::Component::MeshComponent, ECS::Component::ComponentBits::Mesh>::componentHandler;
}

void World::_generateHandlers() {
	// Have to allocate a new handler for each component manually.
	SubComponent<ECS::Component::TransformComponent, 
		ECS::Component::ComponentBits::Transform>::componentHandler
		= new ComponentHandler<ECS::Component::TransformComponent>();

	SubComponent<ECS::Component::MeshComponent,
		ECS::Component::ComponentBits::Mesh>::componentHandler
		= new ComponentHandler<ECS::Component::MeshComponent>();
}


void World::update(float delta) {
	// Update all systems here etc.
	for (auto entity : _entities) {
		printf("%zu\n", entity->id);
		printf("%s\n", entity->getComponent<ECS::Component::TransformComponent>()->type().c_str());
		printf("%s\n", entity->getComponent<ECS::Component::MeshComponent>()->type().c_str());
	}
}

std::shared_ptr<ECS::World::Entity> World::addEntity(const std::string& name) {
	EntityID id = World::_idCounter++;
	auto e = std::make_shared<ECS::World::Entity>();
	e->id = id;
	e->name = name;
	e->addComponent<ECS::Component::TransformComponent>();
	//e->addComponent<ECS::Component::MeshComponent>();
	
	//e->removeComponent<ECS::Component::TransformComponent>();
	_entities.push_back(e);
	_map[id] = _entities.size() - 1;
	return _entities.back();
}

void World::removeEntity(ECS::World::EntityID id) {
	const size_t mapPos = _map[id];
	if (auto toRemove = _entities[mapPos]; 
		toRemove->id != _entities.back()->id) {
		
		_map[_entities.back()->id] = mapPos;
		toRemove.swap(_entities.back());
	}
	auto entity = _entities.back(); // Must be at back now.
	_map.erase(id);
	_entities.pop_back();

	entity.reset(); // Resets the smart pointer, thus destroying the object.
}
