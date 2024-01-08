#include <algorithm>
#include <cmath>

#include "components.hpp"

PlayerComponent::PlayerComponent() {
    component_id = COMP_PLAYER;
    cam = {0};
    cam.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    cam.target = {0,0};
    cam.rotation = 0.0f;
    cam.zoom = 2.0f;
}

MonsterComponent::MonsterComponent(){
    component_id = COMP_OGR;
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
            
            //if(td.collision){
            //    DrawRectangle(x*32,y*32,32,32,RED);
            //}
        }
    }

    for(auto entityId : queried){
        DrawComponent* drawable = static_cast<DrawComponent*>( entityId.GetComponent(COMP_DRAWABLE));
        PositionComponent* position = static_cast<PositionComponent*>( entityId.GetComponent(COMP_POSITION));
        DrawTexture(txt->GetTexture(drawable->text), position->x -16,position->y -16, WHITE);
        //DrawRectangle(position->collision_box.x,position->collision_box.y,position->collision_box.width,position->collision_box.height,RED);
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
        std::cout << "WIECEJ NIZ JEDEN GRACZ!" << std::endl;
        return;
    }
    //Bierzemy EntityId gracza
    auto entityId = queried[0];
    //I bierzemy jego pozycje!
    PositionComponent* position = static_cast<PositionComponent*>( entityId.GetComponent(COMP_POSITION));
    PlayerComponent* player = static_cast<PlayerComponent*>( entityId.GetComponent(COMP_PLAYER));

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

    //Tak takie cos dziala w c++, ale tylko dla jednej linijki B )
    //if(dh != 0 && dv != 0)
    //    ms *= 0.7; // ~~ 1/sqrt(2)

    int tilex = position->x /32;
    int tiley = position->y /32;

    std::vector<Rectangle> tile_collisions;
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
                tile_collisions.push_back(r);
            } 
        }
    }
    int oldposx = position->x;
    int oldposy = position->y;

    Rectangle collision = position->collision_box;
    collision.x += dh * ms;
    collision.y += dv * ms;
    for(Rectangle r : tile_collisions){
        if( r.x < collision.x + collision.width &&
            r.x + r.width > collision.x && 
            r.y < position->collision_box.y + position->collision_box.height &&
            r.y + r.height > position->collision_box.y) {
                dh = 0;
            }
        if(r.x < position->collision_box.x + position->collision_box.width &&
            r.x + r.width > position->collision_box.x && 
            r.y < collision.y + collision.height &&
            r.y + r.height > collision.y){
                dv = 0;
            }
    }
    position->x += dh*ms;
    
    position->y += dv*ms;

    position->collision_box.x = position->x - position->collision_box.width/2;
    position->collision_box.y = position->y - position->collision_box.height/2;

    position->x = std::clamp(position->x,0.0,(double)(t->w*32) - 32);
    position->y = std::clamp(position->y,0.0,(double)(t->h*32) - 32);

    player->cam.target = {  std::clamp((float)position->x + 16, (float)(SCREEN_WIDTH/4), (float)((t->w*32) - SCREEN_WIDTH/4)),
                            std::clamp((float)position->y + 16, (float)(SCREEN_HEIGHT/4), (float)((t->h*32) - SCREEN_HEIGHT/4))};

}

void MonsterSystem::Run(){
    ECS* ecs = ECS::instance();
    if(ecs->GetState() != State::PLAY){
        return;
    }
    auto queriedPlayer = ecs->Query(COMP_PLAYER | COMP_POSITION);
    auto queriedMonster = ecs->Query(COMP_OGR | COMP_POSITION);
    
    if(!queriedPlayer.size() || !queriedMonster.size()){
        return;
    }
    auto playerEntityId = queriedPlayer[0];
    auto playerPosition = static_cast<PositionComponent*>(playerEntityId.GetComponent(COMP_POSITION));

    auto monsterEntityId = queriedMonster[0];    
    PositionComponent* monsterPosition = static_cast<PositionComponent*>( monsterEntityId.GetComponent(COMP_POSITION));
    MonsterComponent* monster = static_cast<MonsterComponent*>( monsterEntityId.GetComponent(COMP_OGR));

    int tilex = monsterPosition->x /32;
    int tiley = monsterPosition->y /32;

    std::vector<Rectangle> tile_collisions;
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
                tile_collisions.push_back(r);
            } 
        }
    }

    

    Vector2 direction = { playerPosition->x - monsterPosition->x, playerPosition->y - monsterPosition->y };
    double kat = atan2(direction.y, direction.x);

    double dh = cos(kat);
    double dv = sin(kat);

    Rectangle collision = monsterPosition->collision_box;
    collision.x += dh * monster->movement_speed;
    collision.y += dv * monster->movement_speed;
    for(Rectangle r : tile_collisions){
        if( r.x < collision.x + collision.width &&
            r.x + r.width > collision.x && 
            r.y < monsterPosition->collision_box.y + monsterPosition->collision_box.height &&
            r.y + r.height > monsterPosition->collision_box.y) {
                dh = 1;
            }
        if(r.x < monsterPosition->collision_box.x + monsterPosition->collision_box.width &&
            r.x + r.width > monsterPosition->collision_box.x && 
            r.y < collision.y + collision.height &&
            r.y + r.height > collision.y){
                dv = 0;
            }
    }

    monsterPosition->x += dh * monster->movement_speed;
    monsterPosition->y += dv * monster->movement_speed;

    monsterPosition->collision_box.x = monsterPosition->x - monsterPosition->collision_box.width / 2;
    monsterPosition->collision_box.y = monsterPosition->y - monsterPosition->collision_box.height / 2;

    monsterPosition->x = std::clamp(monsterPosition->x, 0.0, (double)(ecs->tilemap->w * 32) - 32);
    monsterPosition->y = std::clamp(monsterPosition->y, 0.0, (double)(ecs->tilemap->h * 32) - 32);
}