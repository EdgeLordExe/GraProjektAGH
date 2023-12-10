#include "tilemap.hpp"
#include "entity.hpp"
#include "serialization.hpp"
#include "texture_store.hpp"
#include <algorithm>

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
    uint64_t tid_amt = s.Deserialize<uint64_t>();
    tileDefinitions.clear();
    for(int i = 0 ; i < tid_amt; i ++){
        std::string txt = s.Deserialize<std::string>();
        bool collision = s.Deserialize<bool>();
        textureId id = TextureStore::instance()->LoadTextureWithPath(txt);
        auto id2 = InsertTileDefinition(TileDefinition(id,collision));
        std::cout << txt << " - " << id << " - " << id2<< std::endl;
    }
    for(int i = 0; i < w * h; i ++){
        tiles[i] = s.Deserialize<tileId>();
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
        s.Serialize(txt->GetTextureNameFromId(t.textId));
        s.Serialize(t.collision);
    }
    for(int i = 0; i < w * h; i ++){
        s.Serialize(tiles[i]);
    }
}

Tilemap::Tilemap(uint64_t width, uint64_t height){
    tiles.resize(width * height);
    w = width;
    h = height;
}

tileId Tilemap::GetTile(uint64_t x, uint64_t y){
    return tiles[x + y * w];
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
    tiles[x + y * w] = id;
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