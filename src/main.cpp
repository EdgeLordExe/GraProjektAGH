#include <iostream>
#include "raylib.h"

#include "entity.hpp"
#include "texture_store.hpp"

ECS* ECS::self = NULL;
TextureStore* TextureStore::self = NULL;

int main(){
   
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Orc Hunter");
    SetExitKey(0);

    ECS* world = ECS::instance();

    SetTargetFPS(60);       

    world->Init();     
    
    while (!world->windowExit)    // Detect window close button or ESC key
    {
        world->Tick();
        if(WindowShouldClose()){
            world->windowExit = true;
        }
    }

    CloseWindow();                  // Close window and OpenGL context
    
    return 0;
}
