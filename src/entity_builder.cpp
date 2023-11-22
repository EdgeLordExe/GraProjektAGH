#include "entity_builder.hpp"


EntityBuilder& EntityBuilder::AddComponent(Component* comp){
    components.push_back(comp);
    return *this;
}

EntityId EntityBuilder::Build(){
    DEBG("b1")
    ECS* ecs = ECS::instance();
    DEBG("b2")
    EntityId eid = ecs->MakeEntity();
    DEBG("b3")
    for(Component* comp : components){
        DEBG("b4")
        eid.AddComponent(comp);
        DEBG("b5")
    }
    return eid;
}