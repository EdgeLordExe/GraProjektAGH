#include <cmath>
#include <algorithm>
#include "helpers.hpp"
#include "components.hpp"


Vector2 GetMousePositionInWorld(){
    Vector2 mpos = GetMousePosition();
    ECS* ecs = ECS::instance();
    Vector2 campos = ecs->cam.target;
    return {mpos.x/2 + campos.x - SCREEN_WIDTH/4.0f, mpos.y/2 + campos.y- SCREEN_HEIGHT/4.0f};
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
            if(del_on_hit){
                t->RemoveEntityFromTile(tilex,tiley,id);
                id.Del();
            }
        }
        if(CheckCollisionRecs(r,ycollision)){
            //std::cout << "COLLIDED!" << std::endl;
            //ecs->debug_rectangles.push_back(r);
            ecs->debug_rectangles.push_back(ycollision);
            velocity.y = 0;
            if(del_on_hit){
                t->RemoveEntityFromTile(tilex,tiley,id);
                id.Del();
            }
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