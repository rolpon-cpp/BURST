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
    char texture[32] = {};
    int inventoryIdx = -1;
    bool animating = false;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct WeaponData
{
    char texture[32] = {};
    WeaponType type = NONE;
    float damage = 0.0f;
    float cooldown = 0.0f;
    float range = 0.0f;
    float intensity = 0.0f;
    float spreadAngleRange = 0.0f;
    int shots = 0;
    int ammo = 0;
};
#pragma pack(pop)

class Game;
class Player;
class Inventory;

class Weapon
{
    public:

    Inventory* inventory;

    WeaponData WeaponData;
    float CooldownState;

    Weapon();
    Weapon(Inventory* inventory, struct WeaponData weaponData);
    virtual ~Weapon();
    virtual bool CanAttack();
    virtual void Attack(WeaponAttack attackInfo);
    virtual void Update();
};

class ProjectileWeapon : public Weapon
{
public:
    int Ammo;
    ProjectileWeapon();
    ProjectileWeapon(Inventory* inventory, struct WeaponData weaponData);
    ~ProjectileWeapon();
    bool CanAttack();
    void Attack(WeaponAttack attackInfo) override;
    void Update() override;
};

#define INVENTORY_SIZE 3

class Inventory
{
public:
    Game* game;
    Player* Owner;
    std::shared_ptr<Weapon> Weapons[INVENTORY_SIZE];

    int EquippedItemIdx = -1;

    float WeaponRenderRot = 0.0f;

    Inventory(Game* game, Player* MyPlayerOwner);
    Inventory();
    ~Inventory();

    bool IsHoldingItem();
    void SetItem(std::shared_ptr<Weapon> newWeapon, int Idx);
    void SetItem(WeaponData newWeaponData, int Idx);
    void GiveItem(std::shared_ptr<Weapon> newWeapon);
    void DropItem(int Idx);
    void DropItem();
    void EquipItem(int Idx);
    void UnequipItem();

    void Attack(Vector2 Target);
    void Attack(int Idx, Vector2 Target);
    void Attack(WeaponAttack attackInfo);

    WeaponData GetWeaponData(int Idx);

    void SetCharacterWeaponState();

    void Update();

    void Destroy();
};

#endif //BURST_WEAPON_H