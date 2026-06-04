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
    cout << "attack func started!\n" << flush;
    if (!CanAttack())
        return;
    cout << "CAN attack!\n" << flush;
    this->CooldownState = this->MaxCooldown;
}

void Weapon::Update()
{
    if (CooldownState > 0.0f)
        CooldownState -= inventory->game->GetDeltaTime();
    //cout << CooldownState<< "\n" << flush;
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
    cout << "attack func started!(proj)\n" << flush;
    if (!CanAttack())
        return;
    Weapon::Attack(attackInfo);
    cout << "attack init!\n" << flush;

    if (inventory->game->IsClient)
    {
        GameClient* game_client = (GameClient*) inventory->game;
        game_client->MainClient.AttackWithWeapon(attackInfo);
        cout << "ATTACKINGclient!\n" << flush;
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
                if (plr == this->inventory->Owner)
                    continue;
                if (Vector2Distance(attackInfo.origin, plr->GetCenter()) >= Range)
                    continue;
                RayCastResult result = game_map.CastRay(attackInfo.origin, Angle, Vector2Distance(attackInfo.origin, plr->GetCenter()));

                cout << "BURN THAT MICROFILM BUDDY, WILL YA " <<Vector2Distance(result.hitPositionWorldSpace, plr->GetCenter()) << "\nTARGETS:\nBLACKOPS " <<
                    (result.hitTile == nullptr ? -1 : *result.hitTile)
                    << endl;

                if (result.hitTile == nullptr && Vector2Distance(result.hitPositionWorldSpace, plr->GetCenter()) <= 36.0f)
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

Inventory::Inventory(Game* game, Player* MyPlayerOwner)
{
    //printf("THERE'S A BLUE MOON OUT TONIGHT, %i\n", MyPlayerOwner->PlayerID);
    //std::cout << std::flush;
    this->game = game;
    this->Owner = MyPlayerOwner;
    //printf("THERE'S A SUSSY BLUE MOON OUT TONIGHT, %i\n", this->Owner->PlayerID);
    this->Weapons[0] = nullptr;
    this->Weapons[1] = nullptr;
    this->Weapons[2] = nullptr;
    this->EquippedItemIdx = -1;
}

Inventory::Inventory()
{
    //printf("THERE'S A YELLOW STAR OUT TONIGHT\n");
    //std::cout << std::flush;
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
    if (EquippedItemIdx == Idx)
    {
        UnequipItem();
        return;
    }
    if (Idx < 0 || Idx >=INVENTORY_SIZE)
        return;
    if (Weapons[Idx] != nullptr)
        this->EquippedItemIdx = Idx;
    else
        UnequipItem();
}

void Inventory::UnequipItem()
{
    this->EquippedItemIdx = -1;
}

void Inventory::Attack(int Idx, Vector2 Target)
{
    cout << "IDX CHECK 1\n";
    if (Idx < 0 || Idx >= INVENTORY_SIZE)
        return;
    cout << "IDX CHECK 1 PASSED\n";
    WeaponAttack attackInfo = {
        Owner->GetCenter(),
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
    cout << "IDX CHECK 2\n";
    if (attackInfo.inventoryIdx < 0 || attackInfo.inventoryIdx >= INVENTORY_SIZE)
        return;
    cout << "IDX CHECK 2 PASSED\n";
    cout << "NULLPTR CHECK 1\n";
    if (Weapons[attackInfo.inventoryIdx] != nullptr)
    {
        cout << "NULLPTR CHECK 1 PASSED\n";
        Weapons[attackInfo.inventoryIdx]->Attack(attackInfo);
    } else
        cout << "NULLPTR CHECK 1 FAILED\n";
}

void Inventory::Update()
{
    //printf("processing weapons\n");

    //cout << Owner << "\n" << std::flush;
    //printf("client/keys detection process 1\n");
    if (game != nullptr && game->IsClient)
    {
        if (IsKeyPressed(KEY_ONE))
            EquipItem(0);
        if (IsKeyPressed(KEY_TWO))
            EquipItem(1);
        if (IsKeyPressed(KEY_THREE))
            EquipItem(2);
    }

    //printf("weapons update process 2\n");
    if (EquippedItemIdx != -1 && EquippedItemIdx >= 0 && EquippedItemIdx < INVENTORY_SIZE && Weapons[EquippedItemIdx] != nullptr)
    {
        Weapons[EquippedItemIdx]->inventory = this;
        Weapons[EquippedItemIdx]->Update();
    }

    //printf("owner state mod process 3\n");
    //std::cout << EquippedItemIdx << "\n" << std::flush;
    if (Owner != nullptr && EquippedItemIdx != -1 && Weapons[EquippedItemIdx] != nullptr && EquippedItemIdx >= 0 && EquippedItemIdx < INVENTORY_SIZE)
    {
        //printf("setting state, owner id %i\n", Owner->PlayerID);

        char texture[32];
        strncpy(texture, Weapons[EquippedItemIdx]->Texture.c_str(), sizeof(texture) - 1);
        texture[sizeof(texture) - 1] = '\0'; // ensure null terminator

        //std::cout << std::flush;
        Owner->CurrentState.weapon_state = {
            Weapons[EquippedItemIdx]->Type,
            {},
            EquippedItemIdx,
            false
        };
        memcpy(&Owner->CurrentState.weapon_state.texture, texture, sizeof(texture));
    } else if (Owner != nullptr)
    {
        //std::cout << "THERE'S A RED SPIRAL OUT TONIGHT " << Owner->PlayerID << "\n" << std::flush;
        //std::cout << "THERE'S A RED SPIRAL OUT TO GET THE REST NIGHT " << Owner->CurrentState.position.x << "\n" << std::flush;

        char texture[32];
        texture[0] = '\0'; // ensure null terminator

        //std::cout << std::flush;
        Owner->CurrentState.weapon_state = {
            NONE,
            {},
            -1,
            false
        };
        memcpy(&Owner->CurrentState.weapon_state.texture, texture, sizeof(texture));
    }
    //std::cout << "DONE PROCESSING WEAPONS!" << "\n" << std::flush;

}

void Inventory::Destroy()
{
    //printf("destroying weapons\n");
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        if (Weapons[i] != nullptr)
            Weapons[i].reset();
    }
}
