#ifndef HPP_WEAPONS
#define HPP_WEAPONS
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

typedef uint64_t weaponId;

class Weapon {
    public:
    virtual void Tick(double playerx, double playery);
};

class WeaponRegistry{
    public:
    weaponId RegisterWeapon(Weapon* w);
    Weapon* GetWeaponId(weaponId id);

    private:
    std::vector<std::unique_ptr<Weapon>> weapons;
};



class WeaponSimpleGun : public Weapon{
    public:
    bool automatic;
    int bullet_amt;
    int bullet_spread_arc;
    int bullet_damage;
    int bullet_delay;
    int current_delay;
    int bullet_range;
    int bullet_range_variation;
    int bullet_speed;
    int bullet_speed_variation;
    std::string bullet_texture;
    WeaponSimpleGun(){
        automatic = false;
        bullet_amt = 1;
        bullet_spread_arc = 5;
        bullet_damage = 1;
        bullet_delay = 30; 
        current_delay = 0;
        bullet_range = 512;
        bullet_range_variation = 1;
        bullet_speed = 10;
        bullet_speed_variation = 1;
        bullet_texture = "assets/textures/bullet.png";
    }

    

    virtual void Tick(double playerx, double playery) override;

    virtual void OnLeftMBPressed(double playerx, double playery);
};

class WeaponShotgun : public WeaponSimpleGun{
    public:
    WeaponShotgun() : WeaponSimpleGun(){
        bullet_amt = 15;
        bullet_spread_arc = 25;
        bullet_delay = 120;
        bullet_range = 128;
        bullet_range_variation = 32;
        bullet_speed = 10;
        bullet_speed_variation = 4;
    }
   
};

class WeaponMinigun : public WeaponSimpleGun{
    public:
    WeaponMinigun() : WeaponSimpleGun(){
        automatic = true;
        bullet_amt = 1;
        bullet_spread_arc = 10;
        bullet_delay = 3;
        bullet_range = 256;
        bullet_range_variation = 32;
        bullet_speed = 12;
        bullet_speed_variation = 1;
    }
};

class WeaponCrossbow : public WeaponSimpleGun{
    public:
    WeaponCrossbow() : WeaponSimpleGun(){
        automatic = false;
        bullet_amt = 1;
        bullet_spread_arc = 1;
        bullet_delay = 100;
        bullet_range = 512;
        bullet_range_variation = 1;
        bullet_speed = 16;
        bullet_speed_variation = 1;
        bullet_texture = "assets/textures/bolt.png";
    }
};


#endif