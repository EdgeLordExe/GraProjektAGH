
#include <iostream>
#include "raylib.h"

#include "entity.hpp"
#include "texture_store.hpp"

ECS* ECS::self = NULL;
TextureStore* TextureStore::self = NULL;

int main(){
   

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "gra");

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
