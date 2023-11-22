#ifndef HPP_ENTITY_BUILDER
#define HPP_ENTITY_BUILDER
#include "entity.hpp"

class EntityBuilder{
    std::vector<Component*> components;

    public: 
    EntityBuilder(){};

    EntityBuilder& AddComponent(Component* comp);
    EntityId Build();

};
#endif