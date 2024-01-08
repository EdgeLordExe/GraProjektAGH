#ifndef HPP_WEAPONS
#define HPP_WEAPONS
#include <cstdint>
#include <vector>
#include <memory>

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
    int bullet_amt = 1;
    int bullet_spread_arc = 5;
    int bullet_damage = 1;
    int bullet_delay = 30; 
    int bullet_range = 512;

    virtual void Tick(double playerx, double playery) override;

    virtual void OnLeftMBPressed(double playerx, double playery);
};

#endif