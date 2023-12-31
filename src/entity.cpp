#include <math.h>
//#include <memory>
#include <iostream>

#include "entity.hpp"
#include "components.hpp"
#include "entity_builder.hpp"

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

void ECS::Init(){
    tilemap = std::make_unique<Tilemap>("assets/tilemaps/t1.txt");
    console = std::make_unique<Console>();
    //tilemap->InsertTileDefinition(TileDefinition(TextureStore::instance()->LoadTextureWithPath("assets/textures/t1.png")));

    IncrementComponentStore(3);

    InsertSystem(new PlayerSystem());

    //TO MUSI BYC ZAWSZE OSTATNIE ZAUFAJCIE MI
    InsertSystem(new DrawSystem());

    EntityBuilder().AddComponent(new DrawComponent("assets/textures/player.png"))
                   .AddComponent(new PositionComponent(50,50,8,16))
                   .AddComponent(new PlayerComponent())
                   .Build();

}

/*
    Ta funkcja jest bardzo niebezpieczna, NIGDY nie powinienes zachowywac dlugotrwales kopii wskaznika do Entity
    Jezeli dany obieky zostanie usuniety to wtedy ten pointer będzie nie wazny. To jest tylko do tymczasowego odzyskania jakis danych.

*/
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
        if((entity.component_flags & component_query) == component_query)
            ret.push_back(entity.getSafeId());
    }
    return ret;
}

void ECS::DelEntity(EntityId eid){
    mrkd_for_del.push_back(eid);
}

void ECS::DeleteMarkedEntities(){
    if(!mrkd_for_del.size()){
        return;
    }
    for(auto& eid : mrkd_for_del){
        if(!eid.IsValid()){
            continue;
        }
        Entity& old_entity = entities[eid.id];
        Entity new_entity = Entity();
        new_entity.id = eid.id;
        new_entity.gen = eid.gen + 1;
        entities[eid.id] = new_entity;
        free_ids.push_back(eid.id);
    }
    mrkd_for_del.clear();
}

EntityId ECS::MakeEntity(){
    if(!free_ids.size()){
        Entity entity;
        entity.id = entities.size();
        entities.push_back(entity);
        for(auto& comp_vec : component_store){
            comp_vec.emplace_back(std::unique_ptr<Component>(nullptr));
        }
        return entity.getSafeId();
    }
    entityId free_id = free_ids.back();
    free_ids.pop_back();
    for(auto& comp_vec : component_store){
        comp_vec[free_id] = std::move(std::unique_ptr<Component>(nullptr));
    }
    return entities[free_id].getSafeId();

}