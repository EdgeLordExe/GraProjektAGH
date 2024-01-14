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
    auto* p = GetComponent(COMP_POSITION);
    ECS* ecs = ECS::instance();
    if(p != nullptr){
        auto* cpos = static_cast<PositionComponent*>(p);
        ecs->tilemap->RemoveEntityFromTile(cpos->x/32,cpos->y/32,*this);
    }
    ecs->DelEntity(*this);
}

bool EntityId::HasOneOfComponents(uint64_t compid){
    if(!IsValid()){
        return false;
    }
    ECS* ecs = ECS::instance();
    Entity* entity = ECS::instance()->GetEntity(*this);
    return entity->component_flags & compid;

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
    cam = {0};
    cam.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    cam.target = {0,0};
    cam.rotation = 0.0f;
    cam.zoom = 2.0f;

    tilemap = std::make_unique<Tilemap>("assets/tilemaps/t1.txt");
    console = std::make_unique<Console>();
    weapon_registry = std::make_unique<WeaponRegistry>();
    //tilemap->InsertTileDefinition(TileDefinition(TextureStore::instance()->LoadTextureWithPath("assets/textures/t1.png")));

    weapon_registry->RegisterWeapon(new WeaponCrossbow());
    weapon_registry->RegisterWeapon(new WeaponMinigun());
    weapon_registry->RegisterWeapon(new WeaponShotgun());

    IncrementComponentStore(9);

    InsertSystem(new PlayerSystem());
    InsertSystem(new BulletSystem());
    InsertSystem(new MonsterSystem());
    //TO MUSI BYC ZAWSZE OSTATNIE ZAUFAJCIE MI
    InsertSystem(new DrawSystem());
    InsertSystem(new LucznikSystem());
    InsertSystem(new BiegaczSystem());
    InsertSystem(new TankSystem());

 

    EntityBuilder().AddComponent(new DrawComponent("assets/textures/player.png"))
                   .AddComponent(new PositionComponent(50,50,8,16,16,16))
                   .AddComponent(new PlayerComponent())
                   .Build();



    EntityBuilder().AddComponent(new DrawComponent("assets/textures/ogr.png"))
                   .AddComponent(new PositionComponent(100,100,8,16,16,16))
                   .AddComponent(new MonsterComponent())
                   .Build();

    EntityBuilder().AddComponent(new DrawComponent("assets/textures/lucznik.png"))
                   .AddComponent(new PositionComponent(120,120,8,16,16,16))
                   .AddComponent(new LucznikComponent())
                   .Build();

    EntityBuilder().AddComponent(new DrawComponent("assets/textures/biegacz.png"))
                   .AddComponent(new PositionComponent(140,120,8,16,16,16))
                   .AddComponent(new BiegaczComponent())
                   .Build();
        
    EntityBuilder().AddComponent(new DrawComponent("assets/textures/tank.png"))
                   .AddComponent(new PositionComponent(200,120,16,32,32,32))
                   .AddComponent(new TankComponent())
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
    debug_rectangles.clear();
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
        entity.gen = 0;
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