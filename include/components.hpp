#ifndef HPP_SYSTEMS
#define HPP_SYSTEMS

#include <raylib.h>
#include <unordered_map>
#include <optional>
#include "entity.hpp"
#include "texture_store.hpp"
#include "weapons.hpp"

void MoveAndSlide(EntityId id,Vector2 velocity);

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities);

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities, bool del_on_hit);

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities, bool del_on_hit, uint64_t ignore_entities_with_components);

class PlayerComponent : public Component{
public:
    PlayerComponent();
    double movement_speed = 3.0;
    double sprint_speed = 6.0;

    int max_health = 10;
    int current_health = 10;

    float max_stamina = 100;
    float current_stamina = 100;

    float stamina_regeneration = 0.1;

    std::optional<weaponId> current_weapon = 0;
};

class PositionComponent : public Component{
public:
    PositionComponent(uint64_t xpos, uint64_t ypos,int collider_width, int collider_height, int collider_x_offset, int collider_y_offset);



    virtual void ParseSignal(std::string signal, std::vector<std::any> values) override {};

    double x;
    double y;
    Rectangle collision_box;
    int x_offset;
    int y_offset;
};

class DrawComponent : public Component{
public:
    DrawComponent( std::string path);
    DrawComponent( std::string path, float rotation);
    virtual void ParseSignal(std::string signal, std::vector<std::any> values) override {};

    textureId text;
    float rotation;
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

class BulletComponent : public Component{
    public:
    BulletComponent(float angle, int damage, float speed, int range);

    float angle;
    int damage;
    float speed;
    int range;
    int travelled_range = 0;
};

class BulletSystem: public System{
    virtual void Run() override;
};

#endif