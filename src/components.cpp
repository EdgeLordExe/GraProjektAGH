#include <algorithm>
#include <cmath>



#include "components.hpp"
#include "helpers.hpp"
#include "entity_builder.hpp"

#define MONSTER_PROCESS_RANGE 1048576

PlayerComponent::PlayerComponent() {
    component_id = COMP_PLAYER;
}

sigreturn PlayerComponent::ParseSignal(std::string signal, std::vector<std::any> values){
    if(signal.compare(SIGNAL_HIT) == 0){
        int damage = std::any_cast<int>(values[0]);
        current_health -= damage;
    }
    return 0;
}


MonsterComponent::MonsterComponent(){
    component_id = COMP_OGR;
}

LucznikComponent::LucznikComponent() {
    component_id = COMP_LUCZNIK;
}

BiegaczComponent::BiegaczComponent() {
    component_id = COMP_BIEGACZ;
}

TankComponent::TankComponent(){
    component_id = COMP_TANK;
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


void DrawSystem::Run(){
    ECS* ecs = ECS::instance();
    BeginDrawing();
    ClearBackground(WHITE);
    switch(ecs->GetState()){
        case State::PLAY:
            DrawGame();
        break;
        case State::CONSOLE:
            DrawConsole();
        break;
        case State::MAIN_MENU:
            DrawMainMenu();
        break;
        case State::MAIN_MENU_PLAY:
            DrawMainMenuPlay();
        break;
    }
    EndDrawing();
}

void DrawSystem::DrawMainMenuPlay(){
    TextureStore* txt = TextureStore::instance();
    DrawTexture(txt->GetTexture(txt->LoadTextureWithPath("assets/textures/bg_menu.png")),0,0,WHITE);
    DrawTexture(txt->GetTexture(txt->LoadTextureWithPath("assets/textures/crossbow_menu.png")),0,0,WHITE);
    DrawTexture(txt->GetTexture(txt->LoadTextureWithPath("assets/textures/minigun_menu.png")),0,0,WHITE);
    DrawTexture(txt->GetTexture(txt->LoadTextureWithPath("assets/textures/shotgun_menu.png")),0,0,WHITE);
    if(!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        return;
    }
    Vector2 mpos = GetMousePosition();
    std::cout << "X : " << mpos.x << "Y :" << mpos.y << std::endl;
    ECS* ecs = ECS::instance();
    auto* pcomp = static_cast<PlayerComponent*>(ecs->Query(COMP_PLAYER)[0].GetComponent(COMP_PLAYER));
    if(mpos.x >= 867 && mpos.x <= 1153 && mpos.y >= 217 && mpos.y <= 502){
        pcomp->current_weapon = 1;
        ecs->SwitchState(State::PLAY);
        return;
    }
    if(mpos.x >= 496 && mpos.x <= 783 && mpos.y >= 217 && mpos.y <= 502){
        pcomp->current_weapon = 2;
        ecs->SwitchState(State::PLAY);
        return;
    }
    if(mpos.x >= 126 && mpos.x <= 412 && mpos.y >= 217 && mpos.y <= 502){
        pcomp->current_weapon = 0;
        ecs->SwitchState(State::PLAY);
        return;
    }
}

void DrawSystem::DrawMainMenu(){
    TextureStore* txt = TextureStore::instance();
    DrawTexture(txt->GetTexture(txt->LoadTextureWithPath("assets/textures/bg_menu.png")),0,0,WHITE);
    DrawTexture(txt->GetTexture(txt->LoadTextureWithPath("assets/textures/menu_begin.png")),0,0,WHITE);
    DrawTexture(txt->GetTexture(txt->LoadTextureWithPath("assets/textures/menu_exit.png")),0,0,WHITE);
    //DrawTexture(txt->GetTexture(txt->LoadTextureWithPath("assets/textures/bg_options.png")),0,0,WHITE);
  
    if(!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        return;
    }
    Vector2 mpos = GetMousePosition();
    ECS* ecs = ECS::instance();
    if(mpos.x >= 397 && mpos.x <= 882 && mpos.y >= 301 && mpos.y <= 407){
        ecs->SwitchState(State::MAIN_MENU_PLAY);
        return;
    }
    if(mpos.x >= 397 && mpos.x <= 882 && mpos.y >= 552 && mpos.y <= 659){
        ecs->windowExit = true;
        return;
    }
}

void DrawSystem::DrawConsole(){
    ECS* ecs = ECS::instance();
    ecs->console->Draw();
}

void DrawSystem::DrawGame(){
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
        for(Rectangle r : ecs->debug_rectangles_persistent){
            DrawRectangle(r.x,r.y,r.width,r.height,PURPLE);
        }
    }
    
    EndMode2D();

    //Miejsce na UI
}

void DebugSystem::Run(){
    ECS* ecs = ECS::instance();
    
     if(IsKeyPressed(KEY_GRAVE)){
        if(ecs->GetState() == State::PLAY){
            ecs->SwitchState(State::CONSOLE);
        } else if(ecs->GetState() == State::CONSOLE){
            ecs->SwitchState(State::PLAY);
        }
    }
}

void PlayerSystem::Run(){
    ECS* ecs = ECS::instance();
    auto state = ecs->GetState();
   
    if(state != State::PLAY){
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
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
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

BulletComponent::BulletComponent(float angle, int damage, float speed, int range, bool penetration, int pen_max_mobs, uint64_t comp_ignore)
 : angle(angle), damage(damage), speed(speed), range(range), penetration(penetration), penetration_max_mobs_hit(pen_max_mobs), comp_ignore(comp_ignore){
    component_id = COMP_BULLET;
}

sigreturn BulletComponent::ParseSignal(std::string signal, std::vector<std::any> values){
    //std::cout << "WORKS" << signal << std::endl;
    if(signal.compare(SIGNAL_COLLIDE) != 0){
        return 0;
    }
    EntityId id = std::any_cast<EntityId>(values[0]);
    if(hit_mobs.contains(id.id)){
        return SIGRETURN_COLLIDE_PENETRATE;
    }
    //std::cout << "sending damage:" << damage << " to " << id.id << std::endl;
    id.SendSignal(SIGNAL_HIT,{damage});
    hit_mobs.insert(id.id);

    if(!penetration){
        return 0;
    }

    if(penetration_mobs_hit >= penetration_mobs_hit){
        return 0;
    }

    penetration_mobs_hit++;
    return SIGRETURN_COLLIDE_PENETRATE;
}

void BulletSystem::Run(){
    ECS* ecs = ECS::instance();
    auto query = ecs->Query(COMP_BULLET | COMP_POSITION);
    for(EntityId entityId : query){
        PositionComponent* position = static_cast<PositionComponent*>( entityId.GetComponent(COMP_POSITION));
        BulletComponent* bullet = static_cast<BulletComponent*>( entityId.GetComponent(COMP_BULLET));
        Vector2 vel = {bullet->speed * cos(bullet->angle),bullet->speed * sin(bullet->angle)};
        MoveAndSlide(entityId,vel,true,true,bullet->comp_ignore | COMP_BULLET);
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
    auto playerEntityId = queriedPlayer[0];
    auto playerPosition = static_cast<PositionComponent*>(playerEntityId.GetComponent(COMP_POSITION));

    auto queriedMonster = ecs->Query(COMP_OGR | COMP_POSITION);
     
    if(!queriedPlayer.size() || !queriedMonster.size()){
        return;
    }
  
        for(EntityId monsterEntityId : queriedMonster){
    
        PositionComponent* monsterPosition = static_cast<PositionComponent*>( monsterEntityId.GetComponent(COMP_POSITION));
        MonsterComponent* monster = static_cast<MonsterComponent*>( monsterEntityId.GetComponent(COMP_OGR));
        Vector2 direction = { playerPosition->x - monsterPosition->x, playerPosition->y - monsterPosition->y };
        if(direction.y*direction.y + direction.x*direction.x > MONSTER_PROCESS_RANGE){
            continue;
        }
        double kat = atan2(direction.y, direction.x);

        double dh = cos(kat);
        double dv = sin(kat);
        Vector2 vel = {dh * monster->movement_speed, dv*monster->movement_speed};
        //std::cout << "vel.x : " << vel.x << " vel.y :" << vel.y << std::endl;
        MoveAndSlide(monsterEntityId,vel);
    }
}  

void LucznikSystem::Run(){
    ECS* ecs = ECS::instance();
    if(ecs->GetState() != State::PLAY){
        return;
    }
    auto queriedPlayer = ecs->Query(COMP_PLAYER | COMP_POSITION);
    auto playerEntityId = queriedPlayer[0];
    auto playerPosition = static_cast<PositionComponent*>(playerEntityId.GetComponent(COMP_POSITION));

    auto queriedLucznik = ecs->Query(COMP_LUCZNIK | COMP_POSITION);
    if(!queriedPlayer.size() || !queriedLucznik.size()){
        return;
    }
    for(EntityId lucznikEntityId : queriedLucznik){
        
        PositionComponent* lucznikPosition = static_cast<PositionComponent*>( lucznikEntityId.GetComponent(COMP_POSITION));
        LucznikComponent* lucznik = static_cast<LucznikComponent*>( lucznikEntityId.GetComponent(COMP_LUCZNIK));

        lucznik->arrow_timer++;
        Vector2 direction = { playerPosition->x - lucznikPosition->x, playerPosition->y - lucznikPosition->y };
        if(direction.y*direction.y + direction.x*direction.x > MONSTER_PROCESS_RANGE){
            continue;
        }
        double kat = atan2(direction.y, direction.x);
        double dh = cos(kat);
        double dv = sin(kat);
        if (direction.y*direction.y + direction.x*direction.x > lucznik->range*lucznik->range){
            Vector2 vel = {dh * lucznik->movement_speed, dv*lucznik->movement_speed};
            //std::cout << "vel.x : " << vel.x << " vel.y :" << vel.y << std::endl;
            MoveAndSlide(lucznikEntityId,vel);
        }
        else {
            Vector2 vel = {-dh * lucznik->movement_speed, -dv*lucznik->movement_speed};
            MoveAndSlide(lucznikEntityId,vel);
                if(lucznik->arrow_timer >= 120){
                    lucznik->arrow_timer = 0;
                    EntityBuilder().AddComponent(new DrawComponent("assets/textures/arrow.png",(kat * 360 )/ (2 * PI)))
                            .AddComponent(new PositionComponent(lucznikPosition->x,lucznikPosition->y,4,4,2,2))
                            .AddComponent(new BulletComponent(kat,1,2 ,4,false,0,COMP_LUCZNIK | COMP_BIEGACZ | COMP_OGR | COMP_TANK))
                            .Build();
                }
            }
        }
}  

void BiegaczSystem::Run(){
    ECS* ecs = ECS::instance();
    if(ecs->GetState() != State::PLAY){
        return;
    }
    auto queriedPlayer = ecs->Query(COMP_PLAYER | COMP_POSITION);
    auto playerEntityId = queriedPlayer[0];
    auto playerPosition = static_cast<PositionComponent*>(playerEntityId.GetComponent(COMP_POSITION));

    auto queriedBiegacz = ecs->Query(COMP_BIEGACZ | COMP_POSITION);
    if(!queriedPlayer.size() || !queriedBiegacz.size()){
        return;
    }
    for(EntityId biegaczEntityId : queriedBiegacz){
        
        PositionComponent* biegaczPosition = static_cast<PositionComponent*>( biegaczEntityId.GetComponent(COMP_POSITION));
        BiegaczComponent* biegacz = static_cast<BiegaczComponent*>( biegaczEntityId.GetComponent(COMP_BIEGACZ));
        Vector2 direction = { playerPosition->x - biegaczPosition->x, playerPosition->y - biegaczPosition->y };

        if(direction.y*direction.y + direction.x*direction.x > MONSTER_PROCESS_RANGE){
            continue;
        }

        double kat = atan2(direction.y, direction.x);

        double dh = cos(kat);
        double dv = sin(kat);
        Vector2 vel = {dh * biegacz->movement_speed, dv*biegacz->movement_speed};
        //std::cout << "vel.x : " << vel.x << " vel.y :" << vel.y << std::endl;
        MoveAndSlide(biegaczEntityId,vel);
    }
}  

void TankSystem::Run(){
    ECS* ecs = ECS::instance();
    if(ecs->GetState() != State::PLAY){
        return;
    }
    
    auto queriedPlayer = ecs->Query(COMP_PLAYER | COMP_POSITION);
    auto playerEntityId = queriedPlayer[0];
    auto playerPosition = static_cast<PositionComponent*>(playerEntityId.GetComponent(COMP_POSITION));

    auto queriedTank = ecs->Query(COMP_TANK | COMP_POSITION);
    if(!queriedPlayer.size() || !queriedTank.size()){
        return;
    }
 
    for(EntityId tankEntityId : queriedTank){
    
        PositionComponent* tankPosition = static_cast<PositionComponent*>( tankEntityId.GetComponent(COMP_POSITION));
        TankComponent* tank = static_cast<TankComponent*>( tankEntityId.GetComponent(COMP_TANK));
        Vector2 direction = { playerPosition->x - tankPosition->x, playerPosition->y - tankPosition->y };
        if(direction.y*direction.y + direction.x*direction.x > MONSTER_PROCESS_RANGE){
            continue;
        }
        double kat = atan2(direction.y, direction.x);

        double dh = cos(kat);
        double dv = sin(kat);
        Vector2 vel = {dh * tank->movement_speed, dv*tank->movement_speed};
        //std::cout << "vel.x : " << vel.x << " vel.y :" << vel.y << std::endl;
        MoveAndSlide(tankEntityId,vel);
    }
}  

void DamageableSystem::Run(){
    ECS* ecs = ECS::instance();
    if(ecs->GetState() != State::PLAY){
        return;
    }
    auto q = ecs->Query(COMP_DAMAGEABLE);
    if(!q.size()){
        return;
    }
    for(auto entityId : q){
        DamagableComponent* d = static_cast<DamagableComponent*>( entityId.GetComponent(COMP_DAMAGEABLE));
        if(d->current_health <= 0){
            entityId.Del();
        }
    }
}

DamagableComponent::DamagableComponent(int max_health){
    component_id = COMP_DAMAGEABLE;
    current_health = max_health;
}

sigreturn DamagableComponent::ParseSignal(std::string signal, std::vector<std::any> values){
    if(signal.compare(SIGNAL_HIT) != 0){
        return 0;
    }
    int damage = std::any_cast<int>(values[0]);
    current_health -= damage;
    return 0;
}
