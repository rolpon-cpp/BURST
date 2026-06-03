//
// Created by lalit on 5/20/2026.
//

#include "Weapon.h"

#include <iostream>

#include "../Game.h"

Weapon::Weapon()
{
}

Weapon::Weapon(Inventory* inventory, WeaponType type, std::string texture, float damage, float cooldown)
{
    this->inventory = inventory;
    this->Type = type;
    this->Texture = texture;
    this->Damage = damage;
    this->CooldownState = cooldown;
    this->MaxCooldown = cooldown;
}

Weapon::~Weapon()
{
}

bool Weapon::CanAttack()
{
    if (this->CooldownState > 0.0f)
        return false;

    return true;
}

void Weapon::Attack(WeaponAttack attackInfo)
{
    if (!CanAttack())
        return;
    this->CooldownState = this->MaxCooldown;
}

void Weapon::Update()
{
    if (CooldownState > 0.0f)
        CooldownState -= inventory->game->GetDeltaTime();
}

ProjectileWeapon::ProjectileWeapon()
{
}

ProjectileWeapon::ProjectileWeapon(Inventory* inventory, std::string texture, float damage,
    float cooldown, float range, float spreadAngleRange, int shots, int ammo) : Weapon(inventory,PROJECTILE,texture,damage,cooldown)
{
    this->Range = range;
    this->SpreadAngleRange = spreadAngleRange;
    this->Shots = shots;
    this->Ammo = ammo;
    this->MaxAmmo = ammo;
}

ProjectileWeapon::~ProjectileWeapon()
{
}

bool ProjectileWeapon::CanAttack()
{
    if (!Weapon::CanAttack())
        return false;
    if (this->Ammo <= 0)
        return false;
    return true;
}

void ProjectileWeapon::Attack(WeaponAttack attackInfo)
{
    if (!CanAttack())
        return;
    Weapon::Attack(attackInfo);

    if (inventory->game->IsClient)
    {
        GameClient* game_client = (GameClient*) inventory->game;
        game_client->MainClient.AttackWithWeapon(attackInfo);
    } else
    {
        GameServer* game_server = (GameServer*) inventory->game;
        Map& game_map = game_server->MainMap;

        for (int i = 0; i < Shots; i++)
        {
            float Angle = 180.0f - Vector2LineAngle(attackInfo.origin, attackInfo.target) * RAD2DEG;
            if (SpreadAngleRange > 0.0f)
                Angle += GetRandomValue(-SpreadAngleRange * 5.0f, SpreadAngleRange * 5.0f) / 10.0f;

            for (auto &[id,peer] : game_server->MainServer.Players)
            {
                Player* plr = (Player*) peer->data;
                RayCastResult result = game_map.CastRay(attackInfo.origin, Angle, min(Range, Vector2Distance(attackInfo.origin, plr->CurrentState.position)));
                if (result.hitTile == nullptr && Vector2Distance(result.hitPositionWorldSpace, plr->CurrentState.position) <= 36.0f)
                    plr->CurrentState.health -= Damage;
            }
        }
    }

    this->Ammo -= 1;
}

void ProjectileWeapon::Update()
{
    Weapon::Update();
}

Inventory::Inventory(Game* game, Player* owner)
{
    this->game = game;
    this->Owner = owner;
    this->Weapons[0] = nullptr;
    this->Weapons[1] = nullptr;
    this->Weapons[2] = nullptr;
    this->EquippedItemIdx = -1;
}

Inventory::Inventory()
{
    this->game = nullptr;
    this->Owner = nullptr;
    this->Weapons[0] = nullptr;
    this->Weapons[1] = nullptr;
    this->Weapons[2] = nullptr;
    this->EquippedItemIdx = -1;
}

Inventory::~Inventory()
{
}

bool Inventory::IsHoldingItem()
{
    return EquippedItemIdx != -1;
}

void Inventory::SetItem(std::shared_ptr<Weapon> newWeapon, int Idx)
{
    if (Idx < 0 || Idx >= INVENTORY_SIZE)
        return;
    if (Weapons[Idx] != nullptr)
        Weapons[Idx].reset();
    Weapons[Idx] = std::move(newWeapon);
}

void Inventory::GiveItem(std::shared_ptr<Weapon> newWeapon)
{
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        if (Weapons[i] == nullptr)
        {
            Weapons[i] = newWeapon;
            break;
        }
    }
}

void Inventory::DropItem(int Idx)
{
    if (Idx < 0 || Idx >= INVENTORY_SIZE)
        return;
    if (Weapons[Idx] != nullptr)
        Weapons[Idx].reset();
}

void Inventory::DropItem()
{
    for (int i = INVENTORY_SIZE - 1; i >= 0; i--)
    {
        if (Weapons[i] != nullptr)
        {
            Weapons[i].reset();
            break;
        }
    }
}

void Inventory::EquipItem(int Idx)
{
    if (Idx < 0 || Idx >=INVENTORY_SIZE)
        return;
    if (Weapons[Idx] != nullptr)
        this->EquippedItemIdx = Idx;
}

void Inventory::UnequipItem()
{
    this->EquippedItemIdx = -1;
}

void Inventory::Attack(int Idx, Vector2 Target)
{
    if (Idx < 0 || Idx >= INVENTORY_SIZE)
        return;
    WeaponAttack attackInfo = {
        Owner->CurrentState.position,
        Target,
        Idx,
        ((GameClient*)game)->MainClient.GetServerTime()
    };
    Attack(attackInfo);
}

void Inventory::Attack(Vector2 Target)
{
    Attack(EquippedItemIdx,Target);
}

void Inventory::Attack(WeaponAttack attackInfo)
{
    if (Weapons[attackInfo.inventoryIdx] != nullptr)
        Weapons[attackInfo.inventoryIdx]->Attack(attackInfo);
}

void Inventory::Update()
{
    if (game != nullptr && game->IsClient)
    {
        if (IsKeyPressed(KEY_ONE))
            EquipItem(0);
        if (IsKeyPressed(KEY_TWO))
            EquipItem(1);
        if (IsKeyPressed(KEY_THREE))
            EquipItem(2);
    }

    if (EquippedItemIdx != -1 && EquippedItemIdx >= 0 && EquippedItemIdx < INVENTORY_SIZE)
    {
        Weapons[EquippedItemIdx]->Update();
    }

    if (Owner != nullptr)
    {
        Owner->CurrentState.weapon_state = {
            (EquippedItemIdx < 0 || EquippedItemIdx >= INVENTORY_SIZE || Weapons[EquippedItemIdx] == nullptr) ? NONE : Weapons[EquippedItemIdx]->Type,
                        (EquippedItemIdx < 0 || EquippedItemIdx >= INVENTORY_SIZE || Weapons[EquippedItemIdx] == nullptr) ? "" : Weapons[EquippedItemIdx]->Texture,
                        EquippedItemIdx,
                        false
        };
    }

}

void Inventory::Destroy()
{
    for (int i = 0; i < INVENTORY_SIZE; i++)
        Weapons[i].reset();
}
