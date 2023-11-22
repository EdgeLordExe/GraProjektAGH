#include "entity_builder.hpp"


EntityBuilder& EntityBuilder::AddComponent(Component* comp){
    components.push_back(comp);
    return *this;
}

EntityId EntityBuilder::Build(){
    ECS* ecs = ECS::instance();
    EntityId eid = ecs->MakeEntity();
    for(Component* comp : components){
        eid.AddComponent(comp);
    }
    return eid;
}