#ifndef HPP_ENTITY
#define HPP_ENTITY

#define DEBG(x) std::cout << x << std::endl;

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>
#include <string>
#include <any>

#include "tilemap.hpp"
#include "console.hpp"
#include "weapons.hpp"

typedef uint64_t entityId;

enum State{
    PLAY,
    CONSOLE,
    MAIN_MENU,
    MAIN_MENU_OPTIONS,
    MAIN_MENU_PLAY
};


#define FL_DELETED (1 << 0)

#define COMP_DRAWABLE   (1 << 0)
#define COMP_POSITION   (1 << 1)
#define COMP_PLAYER     (1 << 2)
#define COMP_INSPECT    (1 << 3)
#define COMP_BULLET     (1 << 4)
#define COMP_OGR        (1 << 5)

class Component
{
public:
    Component();
    virtual ~Component();
    virtual void ParseSignal(std::string signal, std::vector<std::any> values){};
    uint64_t GetId(){
        return component_id;
    }

protected:
    uint64_t component_id;
};

class System{
public: 
    System(){};
    virtual ~System(){};

    virtual void Run(){};

};

struct EntityId{
    entityId id;
    uint64_t gen;

    bool operator==(EntityId& rhs){
        return (id == rhs.id && gen == rhs.gen);
    }

    bool IsValid();

    void Del();

    Component* GetComponent(uint64_t compid);

    bool HasOneOfComponents(uint64_t compid);

    uint64_t AddComponent(Component* comp);
    void RemoveComponent(uint64_t compid);
};

struct Entity{
    entityId id;
    uint64_t gen;
    uint64_t flags = 0;
    uint64_t component_flags = 0;

    ~Entity(){};

    EntityId getSafeId(){
        EntityId eid;
        eid.id = id;
        eid.gen = gen;
        return eid;
    }
};

class ECS{
    public:
        ECS(const ECS& obj)= delete; 
        static ECS* instance(){
            if (self == NULL) {
                self = new ECS(); 
                return self; 
            }else{
                return self;
            }
        }

        Entity* GetEntity(EntityId id);

        void Init();
        void Tick();

        std::vector<EntityId> Query(uint64_t component_query);
        void DelEntity(EntityId eid);

        EntityId MakeEntity();

        void InsertSystem(System* system){
            systems.emplace_back(std::unique_ptr<System>(system));
        }

        void IncrementComponentStore(uint64_t num){
            component_store.resize(num);
            for(int i = 0 ; i < num ; i++){
                std::vector<std::unique_ptr<Component>> v{};
                component_store[i] = std::move(v);
            }
        }

        std::vector<std::vector<std::unique_ptr<Component>>> component_store;

        std::unique_ptr<Tilemap> tilemap;
        std::unique_ptr<Console> console;
        std::unique_ptr<WeaponRegistry> weapon_registry;
        Camera2D cam;

        void SwitchState(State newstate){
            //std::cout << "Switching gamestate to " << newstate << std::endl;
            gamestate = newstate;
        };

        State GetState(){
            return gamestate;
        }
        
        //DEBUG VARIABLES
        bool show_hitbox = false;
        std::vector<Rectangle> debug_rectangles;

        bool windowExit = false;

    private: 

        ECS(){};
        static ECS* self;

        State gamestate = State::MAIN_MENU;

        std::vector<EntityId> mrkd_for_del;
        std::vector<Entity> entities;
        std::vector<entityId> free_ids;
        std::vector<std::unique_ptr<System>> systems;

        

        void DeleteMarkedEntities();
};


#endif