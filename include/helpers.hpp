#ifndef HPP_HELPERS 
#define HPP_HELPERS
#include "raylib.h"
#include "entity.hpp"


Vector2 GetMousePositionInWorld(){
    Vector2 mpos = GetMousePosition();
    ECS* ecs = ECS::instance();
    Vector2 campos = ecs->cam.target;
    return {mpos.x/2 + campos.x - SCREEN_WIDTH/4.0f, mpos.y/2 + campos.y- SCREEN_HEIGHT/4.0f};
}

#endif