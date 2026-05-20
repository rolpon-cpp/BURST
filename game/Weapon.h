//
// Created by lalit on 5/20/2026.
//

#ifndef BURST_WEAPON_H
#define BURST_WEAPON_H
#include "../game_libs.h"

enum WeaponType
{
    PROJECTILE, MELEE,
};

struct Projectile
{
    Vector2 launch;
    float damage;
    float rotation;
    float speed;
    double timeShot;
};

class Weapon
{

};


#endif //BURST_WEAPON_H