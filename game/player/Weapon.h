//
// Created by lalit on 5/20/2026.
//

#ifndef BURST_WEAPON_H
#define BURST_WEAPON_H
#include <array>
#include <memory>

#include "../../game_libs.h"
#include "string"

enum WeaponType
{
    NONE, PROJECTILE, MELEE,
};

#pragma pack(push, 1)
struct WeaponAttack
{
    Vector2 origin;
    Vector2 target;
    int inventoryIdx;
    double timestamp;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct WeaponState
{
    WeaponType type = NONE;
    std::string texture = "";
    int inventoryIdx = -1;
    bool animating = false;
};
#pragma pack(pop)

class Game;
class Player;
class Inventory;

class Weapon
{
    public:

    Inventory* inventory = nullptr;

    std::string Texture = "";
    WeaponType Type = NONE;
    float Damage = 0.0f;
    float CooldownState = 0.0f;
    float MaxCooldown = 0.0f;

    Weapon();
    Weapon(Inventory* inventory, WeaponType type, std::string texture, float damage, float cooldown);
    ~Weapon();
    bool CanAttack();
    void Attack(WeaponAttack attackInfo);
    void Update();
};

class ProjectileWeapon : public Weapon
{
public:

    float Range = 0.0f;
    float SpreadAngleRange = 0.0f;
    int Shots = 0;
    int Ammo = 0;
    int MaxAmmo = 0;

    ProjectileWeapon();
    ProjectileWeapon(Inventory* inventory,
        std::string texture, float damage, float cooldown,
        float range, float spreadAngleRange, int shots, int ammo);
    ~ProjectileWeapon();
    bool CanAttack();
    void Attack(WeaponAttack attackInfo);
    void Update();
};

#define INVENTORY_SIZE 3

class Inventory
{
public:
    Game* game = nullptr;
    Player* Owner = nullptr;
    std::shared_ptr<Weapon> Weapons[INVENTORY_SIZE];

    int EquippedItemIdx = -1;

    Inventory(Game* game, Player* owner);
    Inventory();
    ~Inventory();

    bool IsHoldingItem();
    void SetItem(std::shared_ptr<Weapon> newWeapon, int Idx);
    void GiveItem(std::shared_ptr<Weapon> newWeapon);
    void DropItem(int Idx);
    void DropItem();
    void EquipItem(int Idx);
    void UnequipItem();

    void Attack(Vector2 Target);
    void Attack(int Idx, Vector2 Target);
    void Attack(WeaponAttack attackInfo);

    void Update();

    void Destroy();
};

#endif //BURST_WEAPON_H