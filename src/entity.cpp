#include <math.h>
//#include <memory>
#include <iostream>

#include "entity.hpp"
#include "drawable.hpp"

#define FLAG2NUM(x)((uint64_t)floor(log2((double) x )))

Component::Component(){}

Component::~Component(){}

Component* EntityId::GetComponent(uint64_t compid){
    if(!IsValid()){
        return nullptr;
    }
    auto* ecs = ECS::instance();
    auto entity = ecs->GetEntity(*this);
    if(!(entity->component_flags & compid)){
        return nullptr;
    }
    //magiczna instrukcja ktora zamienia compid (2^x) do x 
    return ecs->component_store[FLAG2NUM(compid)][id].get();
}

bool EntityId::IsValid(){
    return ECS::instance()->GetEntity(*this)->gen == gen;
}

void EntityId::Del(){
    ECS::instance()->DelEntity(*this);
}

uint64_t EntityId::AddComponent(Component* comp){
    if(!IsValid()){
        return -1;
    }
    ECS* ecs = ECS::instance();
    ecs->component_store[FLAG2NUM(comp->GetId())][id] = std::unique_ptr<Component>(comp);
    auto compid = comp->GetId();
    ecs->GetEntity(*this)->component_flags |= compid;
    return compid;
}

void EntityId::RemoveComponent(uint64_t compid){
    if(!IsValid()){
        return;
    }
    ECS* ecs = ECS::instance();
    Entity* entity = ECS::instance()->GetEntity(*this);
    ecs->component_store[FLAG2NUM(compid)][id] = std::unique_ptr<Component>(nullptr);
    entity->component_flags &= ~compid;

}


/// ECS
/*
    Ta funkcja jest bardzo niebezpieczna, NIGDY nie powinienes zachowywac dlugotrwales kopii wskaznika do Entity
    Jezeli dany obieky zostanie usuniety to wtedy ten pointer będzie nie wazny. To jest tylko do tymczasowego odzyskania jakis danych.

*/
void ECS::Init(){
    IncrementComponentStore(2);
    InsertSystem(new DrawSystem());
    auto e = MakeEntity();
    e.AddComponent(new DrawComponent("assets/textures/player.png"));
    e.AddComponent(new PositionComponent(50,50));
}

Entity* ECS::GetEntity(EntityId id){
    return &entities[id.id];
}

void ECS::Tick(){
    for(auto& system : systems){
        system->Run();
    }
    DeleteMarkedEntities();
}

std::vector<EntityId> ECS::Query(uint64_t component_query) {
    auto ret = std::vector<EntityId>();
    for(auto& entity : entities){
        if(entity.component_flags & component_query)
            ret.push_back(entity.getSafeId());
    }
    return ret;
}

void ECS::DelEntity(EntityId eid){
    mrkd_for_del.push_back(eid);
}

void ECS::DeleteMarkedEntities(){
    for(auto& eid : mrkd_for_del){
        if(!eid.IsValid()){
            continue;
        }
        Entity& old_entity = entities[eid.id];
        Entity new_entity = Entity();
        new_entity.id = eid.id;
        new_entity.gen = eid.gen + 1;
        entities[eid.id] = new_entity;
    }
    mrkd_for_del.clear();
}

EntityId ECS::MakeEntity(){
    Entity entity;
    entity.id = entities.size();
    entities.push_back(entity);
    for(auto& comp_vec : component_store){
        comp_vec.emplace_back(std::unique_ptr<Component>(nullptr));
    }
    return entity.getSafeId();
}