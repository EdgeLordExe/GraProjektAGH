#include <algorithm>

#include "components.hpp"

PlayerComponent::PlayerComponent() {
    component_id = COMP_PLAYER;
    cam = {0};
    cam.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    cam.target = {0,0};
    cam.rotation = 0.0f;
    cam.zoom = 2.0f;
}

DrawComponent::DrawComponent( std::string path) {
   component_id = COMP_DRAWABLE;
   text = TextureStore::instance()->LoadTextureWithPath(path);
}

PositionComponent::PositionComponent(uint64_t xpos, uint64_t ypos, int collider_width, int collider_height){
    component_id = COMP_POSITION;
    x = xpos;
    y = ypos;
    collision_box = {(float)x,(float)y,(float)collider_width,(float)collider_height};
}

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities){
    ECS* ecs = ECS::instance();
    auto* position = static_cast<PositionComponent*>( id.GetComponent(COMP_POSITION));
    
    
    int tilex = position->x /32;
    int tiley = position->y /32;

    std::vector<Rectangle> collisions;
    std::unique_ptr<Tilemap>& t = ecs->tilemap;
    for(int i = -1; i <= 1; i ++){
        for(int k = -1 ; k <= 1; k ++){
            if((tilex + i < 0) || (tiley + k < 0) || (tilex + i >= t->w) || (tiley +k >= t->h)){
                continue;
            }

            TileDefinition td = t->GetTileDefinition(t->GetTile(tilex + i, tiley + k));
            Rectangle r;
            if(td.collision){
                r.x = (tilex + i) * 32;
                r.y = (tiley + k) * 32;
                r.width = 32;
                r.height = 32;
                collisions.push_back(r);
            } 
            auto entities_on_tile = t->GetEntitiesOnTile(tilex + i,tiley + k);
            if(!collide_with_entities){
                continue;
            }

            for(auto e : entities_on_tile){
                if(e == id){
                    continue; // nie kolidujemy z samym soba!
                }
                auto* epos = static_cast<PositionComponent*>( id.GetComponent(COMP_POSITION));
                collisions.push_back(epos->collision_box);
            }
        }
    }
    int oldposx = position->x;
    int oldposy = position->y;

    Rectangle collision = position->collision_box;
    collision.x += velocity.x;
    collision.y += velocity.y;
    for(Rectangle r : collisions){
        if( r.x < collision.x + collision.width &&
            r.x + r.width > collision.x && 
            r.y < position->collision_box.y + position->collision_box.height &&
            r.y + r.height > position->collision_box.y) {
                velocity.x = 0;
            }
        if(r.x < position->collision_box.x + position->collision_box.width &&
            r.x + r.width > position->collision_box.x && 
            r.y < collision.y + collision.height &&
            r.y + r.height > collision.y){
                velocity.y = 0;
            }
    }
    position->x += velocity.x;
    
    position->y += velocity.y;

    position->collision_box.x = position->x - position->collision_box.width/2;
    position->collision_box.y = position->y - position->collision_box.height/2;

    position->x = std::clamp(position->x,0.0,(double)(t->w*32) - 32);
    position->y = std::clamp(position->y,0.0,(double)(t->h*32) - 32);
    
    if(tilex != (int)(position->x/2) || tiley != (int)(position->y/2)){
        t->RemoveEntityFromTile(tilex,tiley,id);
        t->InsertEntityOnTile(tilex,tiley,id);
    }

}

void DrawSystem::Run(){
    ECS* ecs = ECS::instance();
    TextureStore* txt = TextureStore::instance();
    auto queried = ecs->Query(COMP_DRAWABLE | COMP_POSITION);
    if(!queried.size()){
        return;
    }
    auto qplayer = ecs->Query(COMP_PLAYER);

    if(qplayer.size() != 1){
        return;
    }

    auto player = qplayer[0];
    auto* player_comp = static_cast<PlayerComponent*>( player.GetComponent(COMP_PLAYER));


    BeginDrawing();
    ClearBackground(WHITE);
    BeginMode2D(player_comp->cam);
    for(int x = 0; x < ecs->tilemap->w; x ++){
        for(int y = 0; y < ecs->tilemap->h; y ++){
            tileId tile = ecs->tilemap->GetTile(x,y);
            TileDefinition td = ecs->tilemap->GetTileDefinition(tile);
            textureId texture = td.textId;
            if(!td.autotile){
                DrawTexture(txt->GetTexture(texture),x*32,y*32,WHITE);
            } else {
                Rectangle r = {ecs->tilemap->GetAutotileBitmap(x,y) * 32, 0 , 32 , 32};
                Vector2 v = {(float)x*32,(float)y*32};
                DrawTextureRec(txt->GetTexture(texture),r,v,WHITE);
            }
            
            if(td.collision && ecs->show_hitbox){
                DrawRectangle(x*32,y*32,32,32,RED);
            }
        }
    }

    for(auto entityId : queried){
        DrawComponent* drawable = static_cast<DrawComponent*>( entityId.GetComponent(COMP_DRAWABLE));
        PositionComponent* position = static_cast<PositionComponent*>( entityId.GetComponent(COMP_POSITION));
        DrawTexture(txt->GetTexture(drawable->text), position->x -16,position->y -16, WHITE);
        if(ecs->show_hitbox){
            DrawRectangle(position->collision_box.x,position->collision_box.y,position->collision_box.width,position->collision_box.height,RED);
        }
    }
    
    EndMode2D();
    ecs->console->Draw();
    //Miejsce na UI

    EndDrawing();
}

void PlayerSystem::Run(){
    ECS* ecs = ECS::instance();
    if(ecs->GetState() != State::PLAY){
        return;
    }
    auto queried = ecs->Query(COMP_PLAYER | COMP_POSITION);
    //Nie ma gracza!
    if(!queried.size()){
        return;
    }
    //Jest wiecej niz jeden gracz? tak czy siak to powinno byc nie mozliwe, ale przy duzych projektach niczego nie mozna sie spodziewac
    if(queried.size() > 1){
        return;
    }
    //Bierzemy EntityId gracza
    auto entityId = queried[0];
    //I bierzemy jego pozycje!
    PositionComponent* position = static_cast<PositionComponent*>( entityId.GetComponent(COMP_POSITION));
    PlayerComponent* player = static_cast<PlayerComponent*>( entityId.GetComponent(COMP_PLAYER));
    std::unique_ptr<Tilemap>& t = ecs->tilemap;
    int dh = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    int dv = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);

    // DEBUG
    if(ecs->tilemap->editing){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Vector2 coords = GetMousePosition();
            std::cout << "x :" << coords.x << "y :" << coords.y << std::endl;
            uint64_t worldx = coords.x/2 + player->cam.target.x - SCREEN_WIDTH/4;
            uint64_t worldy = coords.y/2 + player->cam.target.y - SCREEN_HEIGHT/4;

            uint64_t tx = worldx/32;
            uint64_t ty = worldy/32;
            ecs->tilemap->SetTile(tx,ty,ecs->tilemap->brush);
        }
    }
    //END DEBUG


    double ms = player->movement_speed;
    Vector2 vel  ={dh*ms,dv*ms};
    MoveAndSlide(entityId,vel,true);

    player->cam.target = {  std::clamp((float)position->x + 16, (float)(SCREEN_WIDTH/4), (float)((t->w*32) - SCREEN_WIDTH/4)),
                            std::clamp((float)position->y + 16, (float)(SCREEN_HEIGHT/4), (float)((t->h*32) - SCREEN_HEIGHT/4))};

}