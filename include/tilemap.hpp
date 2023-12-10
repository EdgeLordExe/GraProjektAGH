#ifndef HPP_TILEMAP
#define HPP_TILEMAP

#include <cstdint>
#include <tuple>
#include "texture_store.hpp"

typedef uint64_t tileId;

struct TileDefinition{
    tileId tileId;
    textureId textId;

    bool collision = false;

    TileDefinition(textureId tid, bool col);
    TileDefinition(textureId tid);
};

class Tilemap{
public:

    Tilemap(std::string path);
    Tilemap(uint64_t width, uint64_t height);
    tileId InsertTileDefinition(TileDefinition td);

    std::vector<tileId> tiles;
    uint64_t w, h;
    
    tileId brush;
    bool editing = false;

    //std::vector<std::tuple<tileId,double,double>> GetSeenTiles(double x, double y);

    tileId GetTile(uint64_t x, uint64_t y);

    void SetTile(uint64_t x, uint64_t y,tileId id);

    TileDefinition GetTileDefinition(tileId id);

    void Save(std::string path);
    void Load(std::string path);

private:
    
    std::vector<TileDefinition> tileDefinitions;

    
    
};


#endif