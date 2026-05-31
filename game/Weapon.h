//
// Created by lalit on 5/20/2026.
//

#ifndef BURST_WEAPON_H
#define BURST_WEAPON_H
#include "../game_libs.h"
#include "string"

enum WeaponType
{
    NONE, PROJECTILE, MELEE,
};

struct Projectile
{
    Vector2 launch;
    float damage;
    float rotation;
    float speed;
    double timeShot;
};

struct WeaponState
{
    WeaponType type = NONE;
    std::string texture;
};

class Weapon
{
    public:
    std::string Texture;
    WeaponType Type;
    float Damage;

    Weapon();
    ~Weapon();

    void Fire();
};

class ProjectileWeapon : Weapon
{

};

#endif //BURST_WEAPON_H