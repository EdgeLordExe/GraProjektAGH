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
    if(current_delay > 0){
        current_delay -= 1;
    }
    if((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && current_delay == 0 && automatic == false) || (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && current_delay == 0 && automatic == true )){
        OnLeftMBPressed(playerx, playery);
        current_delay = bullet_delay;
    }

}

void WeaponSimpleGun::OnLeftMBPressed(double playerx, double playery){
    Vector2 pos = GetMousePositionInWorld();
    ECS* ecs = ECS::instance();
    for(int i = 0; i < bullet_amt; i ++){
        float x = pos.x - playerx;
        float y = pos.y - playery;
        float angle = atan2f(y,x);
        angle += (((rand() % (2 * bullet_spread_arc)) - bullet_spread_arc) * 2 * PI) / 360;
        EntityBuilder().AddComponent(new DrawComponent(bullet_texture,(angle * 360 )/ (2 * PI)))
                        .AddComponent(new PositionComponent(playerx,playery,4,4,2,2))
                        .AddComponent(new BulletComponent(angle,bullet_damage,bullet_speed + (rand() % (2*bullet_speed_variation)) - bullet_speed_variation ,bullet_range + (rand() % (2*bullet_range_variation)) - bullet_range_variation,bullet_penetration,bullet_max_mobs_penetrated))
                        .Build();
    }

};
