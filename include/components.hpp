#ifndef HPP_SYSTEMS
#define HPP_SYSTEMS

#include <raylib.h>
#include <unordered_map>
#include "entity.hpp"
#include "texture_store.hpp"

class PlayerComponent : public Component{
public:
    PlayerComponent();
    Camera2D cam;
    double movement_speed = 6.0;
};

class PositionComponent : public Component{
public:
    PositionComponent(uint64_t xpos, uint64_t ypos,int collider_width, int collider_height);

    virtual void ParseSignal(std::string signal, std::vector<std::any> values) override {};

    double x;
    double y;
    Rectangle collision_box;
};

class DrawComponent : public Component{
public:
    DrawComponent( std::string path);
    virtual void ParseSignal(std::string signal, std::vector<std::any> values) override {};

    textureId text;
};

class DrawSystem : public System{
    virtual void Run() override;
};

class PlayerSystem : public System{
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