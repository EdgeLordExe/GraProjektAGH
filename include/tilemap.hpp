#ifndef HPP_TILEMAP
#define HPP_TILEMAP

#include <cstdint>
#include <tuple>
#include "texture_store.hpp"

typedef uint64_t tileId;

#define TOP (1 << 0)
#define RIGHT (1 << 1)
#define BOTTOM (1 << 2)
#define LEFT (1 << 3)


struct TileDefinition{
    tileId tileId;
    textureId textId;

    bool collision = false;

    bool autotile = false;

    TileDefinition(textureId tid, bool col, bool autot);
    TileDefinition(textureId tid, bool col);
    TileDefinition(textureId tid);
};

class Tilemap{
public:

    Tilemap(std::string path);
    Tilemap(uint64_t width, uint64_t height);
    tileId InsertTileDefinition(TileDefinition td);

    std::vector<tileId> tiles;
    std::vector<char> autotiles_bitmap;
    uint64_t w, h;
    
    tileId brush;
    bool editing = false;

    //std::vector<std::tuple<tileId,double,double>> GetSeenTiles(double x, double y);

    tileId GetTile(uint64_t x, uint64_t y);

    void SetTile(uint64_t x, uint64_t y,tileId id);

    void SetAutotileBitmap(uint64_t x, uint64_t y, char value);

    char GetAutotileBitmap(uint64_t x, uint64_t y);

    TileDefinition GetTileDefinition(tileId id);

    void Save(std::string path);
    void Load(std::string path);

private:
    
    std::vector<TileDefinition> tileDefinitions;

    
    
};


#endif