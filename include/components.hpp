#ifndef HPP_SYSTEMS
#define HPP_SYSTEMS

#include <raylib.h>
#include <unordered_map>
#include "entity.hpp"

class PlayerComponent : public Component{
    public:
        PlayerComponent();
};

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
    virtual ~DrawComponent() override;
    virtual void ParseSignal(std::string signal, std::vector<std::any> values) override {};

    Texture2D text;
};

class DrawSystem : public System{
    virtual void Run() override;
};

class InspectComponent : public Component{
    InspectComponent(std::unordered_map<std::string,std::string> properties){
        component_id = COMP_INSPECT;
    }

    std::string GetProperty(std::string key){
        return internal.count(key) == 1 ? internal[key] : "";
    }

    private: 
    std::unordered_map<std::string,std::string> internal;

};


#endif