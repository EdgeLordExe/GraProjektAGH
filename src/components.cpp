#include <algorithm>
#include <cmath>

#include "components.hpp"

PlayerComponent::PlayerComponent() {
    component_id = COMP_PLAYER;
}

MonsterComponent::MonsterComponent(){
    component_id = COMP_OGR;
}

DrawComponent::DrawComponent( std::string path) {
   component_id = COMP_DRAWABLE;
   text = TextureStore::instance()->LoadTextureWithPath(path);
   rotation = 0;
}

DrawComponent::DrawComponent( std::string path, float rotation): rotation(rotation) {
   component_id = COMP_DRAWABLE;
   text = TextureStore::instance()->LoadTextureWithPath(path);
}

PositionComponent::PositionComponent(uint64_t xpos, uint64_t ypos, int collider_width, int collider_height, int collider_x_offset, int collider_y_offset){
    component_id = COMP_POSITION;
    x = xpos;
    y = ypos;
    x_offset = collider_x_offset;
    y_offset = collider_y_offset;
    collision_box = {(float)x - collider_x_offset,(float)y - collider_y_offset,(float)collider_width,(float)collider_height};
}

void MoveAndSlide(EntityId id,Vector2 velocity){
    MoveAndSlide(id,velocity,true,false,0);
}

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities){
    MoveAndSlide(id,velocity,collide_with_entities,false,0);
}

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities,bool del_on_hit){
    MoveAndSlide(id,velocity,collide_with_entities,del_on_hit,0);
}

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities,bool del_on_hit, uint64_t ignore_entities_with_components){
    ECS* ecs = ECS::instance();
    auto* position = static_cast<PositionComponent*>( id.GetComponent(COMP_POSITION));
    
    
    int tilex = position->x /32;
    int tiley = position->y /32;
    //std::cout << "x : " << position->x  << " y :" << position->y << std::endl;

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
            if(!collide_with_entities){
                continue;
            }
            auto entities_on_tile = t->GetEntitiesOnTile(tilex + i,tiley + k);


            for(auto e : entities_on_tile){
                //std::cout << "e.id : " << e.id << " e.gen :" << e.gen << std::endl;
                if(!e.IsValid()){
                    continue;
                }
                if(e == id){
                    continue; // nie kolidujemy z samym soba!
                }
                if(ignore_entities_with_components != 0 && e.HasOneOfComponents(ignore_entities_with_components)){
                    continue;
                }
                //std::cout << "ADDING TO LIST AN ENTITY WOO HOO!" << std::endl;
                auto* epos = static_cast<PositionComponent*>( e.GetComponent(COMP_POSITION));
                collisions.push_back(epos->collision_box);
            }
        }
    }

    Rectangle xcollision = position->collision_box;
    Rectangle ycollision = position->collision_box;

    xcollision.x += velocity.x;
    ycollision.y += velocity.y;

    for(Rectangle r : collisions){
        if(CheckCollisionRecs(r,xcollision)){
            //std::cout << "COLLIDED!" << std::endl;
            //ecs->debug_rectangles.push_back(r);
            ecs->debug_rectangles.push_back(xcollision);
            velocity.x = 0;
        }
        if(CheckCollisionRecs(r,ycollision)){
            //std::cout << "COLLIDED!" << std::endl;
            //ecs->debug_rectangles.push_back(r);
            ecs->debug_rectangles.push_back(ycollision);
            velocity.y = 0;
        }
    }
    position->x += velocity.x;
    
    position->y += velocity.y;


    if(!del_on_hit){
        position->x = std::clamp(position->x,0.0,(double)(t->w*32) - 32);
        position->y = std::clamp(position->y,0.0,(double)(t->h*32) - 32);
    } else {
        if(position->x <= 0 || position->x >= ((t->w*32) - 64) ){
            t->RemoveEntityFromTile(tilex,tiley,id);
            id.Del();
            return;
        }
        if(position->y <= 0 || position->y >= ((t->h*32) -64) ){
            t->RemoveEntityFromTile(tilex,tiley,id);
            id.Del();
            return;
        }
    }
    position->collision_box.x = position->x - position->collision_box.width/2;
    position->collision_box.y = position->y - position->collision_box.height/2;

    if(tilex != (int)(position->x/32) || tiley != (int)(position->y/32)){
        t->RemoveEntityFromTile(tilex,tiley,id);
        t->InsertEntityOnTile((int)(position->x/32),(int)(position->y/32),id);
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
    BeginMode2D(ecs->cam);
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
        Vector2 pos = {position->x - position->x_offset,position->y - position->y_offset};
        DrawTextureEx(txt->GetTexture(drawable->text), pos ,drawable->rotation,1, WHITE);
        if(ecs->show_hitbox){
            DrawRectangle(position->collision_box.x,position->collision_box.y,position->collision_box.width,position->collision_box.height,RED);
            DrawCircle(position->x,position->y,2,BLUE);
        }
    }
    if(ecs->show_hitbox){
        for(Rectangle r : ecs->debug_rectangles){
            DrawRectangle(r.x,r.y,r.width,r.height,GREEN);
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
        std::cout << "WIECEJ NIZ JEDEN GRACZ!" << std::endl;
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
            uint64_t worldx = coords.x/2 + ecs->cam.target.x - SCREEN_WIDTH/4;
            uint64_t worldy = coords.y/2 + ecs->cam.target.y - SCREEN_HEIGHT/4;

            uint64_t tx = worldx/32;
            uint64_t ty = worldy/32;
            ecs->tilemap->SetTile(tx,ty,ecs->tilemap->brush);
        }
    }
    //END DEBUG


    double ms = player->movement_speed;
    Vector2 vel  ={dh*ms,dv*ms};
    MoveAndSlide(entityId,vel,true,false,COMP_BULLET);

    ecs->cam.target = {  std::clamp((float)position->x + 16, (float)(SCREEN_WIDTH/4), (float)((t->w*32) - SCREEN_WIDTH/4)),
                            std::clamp((float)position->y + 16, (float)(SCREEN_HEIGHT/4), (float)((t->h*32) - SCREEN_HEIGHT/4))};


    //Bronie!

    if(player->current_weapon.has_value()){
        weaponId w = player->current_weapon.value();
        ecs->weapon_registry->GetWeaponId(w)->Tick(position->x, position->y);
    }

}

BulletComponent::BulletComponent(float angle, int damage, float speed, int range) : angle(angle), damage(damage), speed(speed), range(range){
    component_id = COMP_BULLET;
}

void BulletSystem::Run(){
    ECS* ecs = ECS::instance();
    auto query = ecs->Query(COMP_BULLET | COMP_POSITION);
    for(EntityId entityId : query){
        PositionComponent* position = static_cast<PositionComponent*>( entityId.GetComponent(COMP_POSITION));
        BulletComponent* bullet = static_cast<BulletComponent*>( entityId.GetComponent(COMP_BULLET));
        Vector2 vel = {bullet->speed * cos(bullet->angle),bullet->speed * sin(bullet->angle)};
        MoveAndSlide(entityId,vel,true,true,COMP_PLAYER | COMP_BULLET);
        if(!entityId.IsValid()){
            continue;
        }
        bullet->travelled_range += bullet->speed;
        if(bullet->travelled_range > bullet->range){
            entityId.Del();
        }
    }
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
    Vector2 direction = { playerPosition->x - monsterPosition->x, playerPosition->y - monsterPosition->y };
    double kat = atan2(direction.y, direction.x);

    double dh = cos(kat);
    double dv = sin(kat);
    Vector2 vel = {dh * monster->movement_speed, dv*monster->movement_speed};
    //std::cout << "vel.x : " << vel.x << " vel.y :" << vel.y << std::endl;
    MoveAndSlide(monsterEntityId,vel);
}