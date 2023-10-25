#ifndef HPP_SYSTEMS
#define HPP_SYSTEMS

#include <raylib.h>
#include "entity.hpp"

class PositionComponent : public Component{
public:
    PositionComponent(uint64_t xpos, uint64_t ypos);

    virtual void ParseSignal(std::string signal, std::vector<std::any> values) override {};

    uint64_t x;
    uint64_t y;
};

class DrawComponent : public Component{
public:
    DrawComponent( std::string path);
    virtual ~DrawComponent() override {};
    virtual void ParseSignal(std::string signal, std::vector<std::any> values) override {};

    Texture2D text;
};

class DrawSystem : public System{
    virtual void Run() override;
};


#endif