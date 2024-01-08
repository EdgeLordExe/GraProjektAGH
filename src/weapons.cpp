#include <cmath>
#include <iostream>
#include "raylib.h"

#include "weapons.hpp"
#include "helpers.hpp"
#include "entity_builder.hpp"
#include "components.hpp"

weaponId WeaponRegistry::RegisterWeapon(Weapon* w){
    weaponId id = weapons.size();
    weapons.push_back(std::unique_ptr<Weapon>(w));
    return id;
}

Weapon* WeaponRegistry::GetWeaponId(weaponId id){
    return weapons[id].get();
}


void Weapon::Tick(double playerx, double playery){}

void WeaponSimpleGun::Tick(double playerx, double playery){
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        OnLeftMBPressed(playerx, playery);
    }
}

void WeaponSimpleGun::OnLeftMBPressed(double playerx, double playery){
    Vector2 pos = GetMousePositionInWorld();
    ECS* ecs = ECS::instance();
    for(int i = 0; i < bullet_amt; i ++){
        float x = pos.x - playerx;
        float y = pos.y - playery;
        float angle = atan2f(y,x);
        angle += (rand() % (2 * bullet_spread_arc)) - bullet_spread_arc
        EntityBuilder().AddComponent(new DrawComponent("assets/textures/bullet.png"))
                        .AddComponent(new PositionComponent(playerx+16,playery+16,4,4))
                        .AddComponent(new BulletComponent(angle,bullet_damage,10,bullet_range))
                        .Build();
    }

};