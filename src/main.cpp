
#include <iostream>
#include "raylib.h"

#include "entity.hpp"

ECS* ECS::self = NULL;
int main(){
   
    
    InitWindow(640, 480, "raylib");
    
    ECS* world = ECS::instance();
    SetTargetFPS(60);       
    world->Init();     
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        world->Tick();
    }

    CloseWindow();                  // Close window and OpenGL context
    
    return 0;
}
