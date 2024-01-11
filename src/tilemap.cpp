#include "tilemap.hpp"
#include "entity.hpp"
#include "serialization.hpp"
#include "texture_store.hpp"
#include <algorithm>

TileDefinition::TileDefinition(textureId tid, bool col, bool autot){
    collision = col;
    autotile = autot;
    textId = tid;
}

TileDefinition::TileDefinition(textureId tid, bool col){
    textId = tid;
    collision = col;
}

TileDefinition::TileDefinition(textureId tid){
    textId = tid;
    collision = false;
}

Tilemap::Tilemap(std::string path){
    Load(path);
    
}

void Tilemap::Load(std::string path){
    Deserializer s(path);
    w = s.Deserialize<uint64_t>();
    h = s.Deserialize<uint64_t>();
    tiles.resize(w * h);
    autotiles_bitmap.resize(w * h);
    entities.resize(w * h);
    uint64_t tid_amt = s.Deserialize<uint64_t>();
    tileDefinitions.clear();
    for(int i = 0 ; i < tid_amt; i ++){
        bool autotile = s.Deserialize<bool>();
        std::string txt = s.Deserialize<std::string>();
        bool collision = s.Deserialize<bool>();
        textureId id = TextureStore::instance()->LoadTextureWithPath(txt);
        auto id2 = InsertTileDefinition(TileDefinition(id,collision,autotile));
        std::cout << txt << " - " << id << " - " << id2<< std::endl;
    }
    for(int i = 0; i < w * h; i ++){
        tiles[i] = s.Deserialize<tileId>();
    }
    for(int i = 0; i < w * h; i ++){
        autotiles_bitmap[i] = (char)s.Deserialize<int>();
    }
}

void Tilemap::Save(std::string path){
    Serializer s(path);
    s.Serialize(w);
    s.Serialize(h);
    s.Serialize(tileDefinitions.size());
    TextureStore* txt = TextureStore::instance();
    for(int i = 0 ; i < tileDefinitions.size(); i ++){
        TileDefinition t = tileDefinitions[i];
        s.Serialize(t.autotile);
        s.Serialize(txt->GetTextureNameFromId(t.textId));
        s.Serialize(t.collision);
        
    }
    for(int i = 0; i < w * h; i ++){
        s.Serialize(tiles[i]);
    }
    for(int i = 0 ; i < w*h; i ++){
        s.Serialize((int)autotiles_bitmap[i]);
    }
}

Tilemap::Tilemap(uint64_t width, uint64_t height){
    tiles.resize(width * height);
    autotiles_bitmap.resize(width * height);
    w = width;
    h = height;
}

tileId Tilemap::GetTile(uint64_t x, uint64_t y){
    return tiles[x + y * w];
}

char Tilemap::GetAutotileBitmap(uint64_t x, uint64_t y){
    return autotiles_bitmap[x + y * w];
}

void Tilemap::SetAutotileBitmap(uint64_t x, uint64_t y, char value){
    autotiles_bitmap[x + y * w] = value; 
}

tileId Tilemap::InsertTileDefinition(TileDefinition td){
    td.tileId = tileDefinitions.size();
    std::cout << "Inserting tile :" << td.tileId << " / " << td.textId << " / " << td.collision << std::endl;
    tileDefinitions.push_back(td);
    return td.tileId;
}

TileDefinition Tilemap::GetTileDefinition(tileId id){
    return tileDefinitions[id];
}

void Tilemap::SetTile(uint64_t x, uint64_t y,tileId id){
    tileId oldId = GetTile(x,y);
    if(!GetTileDefinition(id).autotile && !GetTileDefinition(oldId).autotile){
        tiles[x + y * w] = id;
        return;
    }
    char autotile_bitmap = 0 ;

    if((x + 1 < w) && (GetTile(x + 1, y) == oldId)){
        SetAutotileBitmap(x + 1, y, GetAutotileBitmap(x + 1, y) & ~LEFT);
    }
    if((x - 1 >= 0) && (GetTile(x - 1, y) == oldId)){
        SetAutotileBitmap(x - 1, y, GetAutotileBitmap(x - 1, y) & ~RIGHT);
    }
    if((y + 1 < h) && (GetTile(x, y+1) == oldId)){
        SetAutotileBitmap(x, y+1, GetAutotileBitmap(x, y+1) & ~TOP);
    }
    if((y - 1 >= 0) && (GetTile(x, y - 1) == oldId)){
        SetAutotileBitmap(x, y-1, GetAutotileBitmap(x, y-1) & ~BOTTOM);
    }
    if((x + 1 < w) && (GetTile(x + 1, y) == id)){
        SetAutotileBitmap(x + 1, y, GetAutotileBitmap(x + 1, y) | LEFT);
        autotile_bitmap |= RIGHT;
    }
    if((x - 1 >= 0) && (GetTile(x - 1, y) == id)){
        SetAutotileBitmap(x - 1, y, GetAutotileBitmap(x - 1, y) | RIGHT);
        autotile_bitmap |= LEFT;
    }
    if((y + 1 < h) && (GetTile(x, y+1) == id)){
        SetAutotileBitmap(x, y+1, GetAutotileBitmap(x, y+1) | TOP);
        autotile_bitmap |= BOTTOM;
    }
    if((y - 1 >= 0) && (GetTile(x, y - 1) == id)){
        SetAutotileBitmap(x, y-1, GetAutotileBitmap(x, y-1) | BOTTOM);
        autotile_bitmap |= TOP;
    }
    SetAutotileBitmap(x,y,autotile_bitmap);
    tiles[x + y * w] = id;
}

std::vector<EntityId>& Tilemap::GetEntitiesOnTile(uint64_t x, uint64_t y){
    return entities[x + y * w];
}

void Tilemap::InsertEntityOnTile(uint64_t x, uint64_t y, EntityId id){
    entities[x + y * w].push_back(id);
}

void Tilemap::RemoveEntityFromTile(uint64_t x, uint64_t y, EntityId id){
    for(int i = 0 ; i < entities[x + y * w].size(); i ++){
        if(entities[x + y * w][i] == id){
            //std::cout << "REMOVED ENTITY!" << std::endl;
            entities[x + y * w].erase(entities[x + y * w].begin() + i);
        }
    }
}


/*
std::vector<std::tuple<tileId,double,double>> Tilemap::GetSeenTiles(double x, double y){
    std::vector<std::tuple<tileId,double,double>> query;
    uint64_t width = (SCREEN_WIDTH / 32) + 2;
    uint64_t height = (SCREEN_HEIGHT / 32) + 2;
    //query.resize(width * height);

    uint64_t from_x = std::max(0, (int)(x - width/2));
    uint64_t from_y = std::max(0, (int)(y - height/2));

    uint64_t to_x = std::min(w,(uint64_t)(x + width/2));
    uint64_t to_y = std::min(h,(uint64_t)(y + width/2));

    for(int i = from_x; i < to_x; i ++){
        for(int k = from_y; k < to_y; k ++){
            double offset
            query.push_back(std::make_tuple(GetTile(i,k),));
        }
    }
}
*/