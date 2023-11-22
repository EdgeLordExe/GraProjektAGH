#include "components.hpp"
PlayerComponent::PlayerComponent() {
   component_id = COMP_PLAYER;
}

DrawComponent::DrawComponent( std::string path) {
   component_id = COMP_DRAWABLE;
   text = LoadTexture(path.c_str());
}

DrawComponent::~DrawComponent(){
    UnloadTexture(text);
}

PositionComponent::PositionComponent(uint64_t xpos, uint64_t ypos){
    component_id = COMP_POSITION;
    x = xpos;
    y = ypos;
}

void DrawSystem::Run(){
    BeginDrawing();
    ClearBackground(WHITE);
    ECS* ecs = ECS::instance();
    auto queried = ecs->Query(COMP_DRAWABLE | COMP_POSITION);
    if(!queried.size()){
        return;
    }
    for(auto entityId : queried){
        DrawComponent* drawable = static_cast<DrawComponent*>( entityId.GetComponent(COMP_DRAWABLE));
        PositionComponent* position = static_cast<PositionComponent*>( entityId.GetComponent(COMP_POSITION));
        DrawTexture(drawable->text, position->x ,position->y , WHITE);
    }
    EndDrawing();
}