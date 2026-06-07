//
// Created by lalit on 5/20/2026.
//

#include "Weapon.h"

#include <iostream>

#include "../Game.h"

float LerpAngle(float current, float target, float t) {
    float diff = fmodf(target - current + 540.0f, 360.0f) - 180.0f;
    return current + diff * t;
}

Weapon::Weapon()
{
}

Weapon::Weapon(Inventory* inventory, struct WeaponData weaponData)
{
    this->inventory = inventory;
    this->WeaponData = weaponData;
    this->CooldownState = 0.0f;
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
    this->CooldownState = this->WeaponData.cooldown;
}

void Weapon::Update()
{
    if (CooldownState > 0.0f)
        CooldownState -= inventory->game->GetDeltaTime();

    std::string c = to_string(*WeaponData.texture);

    //cout << CooldownState<< "\n" << flush;
}

ProjectileWeapon::ProjectileWeapon()
{
}

ProjectileWeapon::ProjectileWeapon(Inventory* inventory, struct WeaponData weapon_data) : Weapon(inventory,weapon_data)
{
    Ammo = weapon_data.ammo;
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

        for (int i = 0; i < WeaponData.shots; i++)
        {
            float Angle = 180.0f - Vector2LineAngle(attackInfo.origin, attackInfo.target) * RAD2DEG;
            if (WeaponData.spreadAngleRange > 0.0f)
                Angle += GetRandomValue(-WeaponData.spreadAngleRange * 5.0f, WeaponData.spreadAngleRange * 5.0f) / 10.0f;

            for (auto &[id,peer] : game_server->MainServer.Players)
            {
                Player* plr = (Player*) peer->data;
                if (plr == this->inventory->Owner)
                    continue;
                if (Vector2Distance(attackInfo.origin, plr->GetCenter()) >= WeaponData.range)
                    continue;
                RayCastResult result = game_map.CastRay(attackInfo.origin, Angle, Vector2Distance(attackInfo.origin, plr->GetCenter()));

                if (result.hitTile == nullptr && Vector2Distance(result.hitPositionWorldSpace, plr->GetCenter()) <= 25.45f)
                    plr->CurrentState.health -= WeaponData.damage;
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

void Inventory::SetItem(WeaponData newWeaponData, int Idx)
{
    std::shared_ptr<Weapon> wep = nullptr;
    if (newWeaponData.type == NONE)
    {
        DropItem(Idx);
        return;
    }
    if (newWeaponData.type == PROJECTILE)
        wep = make_shared<ProjectileWeapon>(this, newWeaponData);
    else
        wep = make_shared<Weapon>(this, newWeaponData);
    if (wep != nullptr)
        SetItem(wep, Idx);
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
    if (Idx < 0 || Idx >= INVENTORY_SIZE)
        return;
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
    if (attackInfo.inventoryIdx < 0 || attackInfo.inventoryIdx >= INVENTORY_SIZE)
        return;
    if (Weapons[attackInfo.inventoryIdx] != nullptr)
    {
        Weapons[attackInfo.inventoryIdx]->Attack(attackInfo);
    }
}

void Inventory::Update()
{
    SetCharacterWeaponState();

    if (game != nullptr && game->IsClient)
    {
        if (Owner->IsLocalPlayer())
        {
            if (Owner->CurrentState.health > 0)
            {
                if (IsKeyPressed(KEY_ONE))
                    EquipItem(0);
                if (IsKeyPressed(KEY_TWO))
                    EquipItem(1);
                if (IsKeyPressed(KEY_THREE))
                    EquipItem(2);
            } else
            {
                UnequipItem();

            }
        }

        std::string c = Owner->CurrentState.weapon_state.texture;

        if (!c.empty())
        {
            GameClient* game_c = (GameClient*)game;

            WeaponRenderRot = LerpAngle(WeaponRenderRot, Owner->CurrentState.rotation, 24.0f * game->GetDeltaTime());

            Vector2 offset = {
                cosf(WeaponRenderRot * DEG2RAD) * 100.0f,
                sinf(WeaponRenderRot * DEG2RAD) * 100.0f
            };

            Texture2D& g = game_c->MainResources.GetTexture(c);
            DrawTexturePro(g, {0, 0, (float) g.width, (float) g.height}, {
                Owner->GetCenter().x - offset.x, Owner->GetCenter().y - offset.y,
                (float)g.width * 3.0f, (float)g.height * 3.0f,
            }, {g.width * 1.5f, g.height * 1.5f}, WeaponRenderRot, WHITE);
        }
    }

    //printf("weapons update process 2\n");
    if (EquippedItemIdx != -1 && EquippedItemIdx >= 0 && EquippedItemIdx < INVENTORY_SIZE && Weapons[EquippedItemIdx] != nullptr)
    {
        Weapons[EquippedItemIdx]->inventory = this;
        Weapons[EquippedItemIdx]->Update();
    }

}

void Inventory::SetCharacterWeaponState()
{
    if (!Owner->IsLocalPlayer())
        return;
    if (Owner != nullptr && EquippedItemIdx != -1 && Weapons[EquippedItemIdx] != nullptr && EquippedItemIdx >= 0 && EquippedItemIdx < INVENTORY_SIZE)
    {
        Owner->CurrentState.weapon_state = {
            Weapons[EquippedItemIdx]->WeaponData.type,
            {},
            EquippedItemIdx,
            false
        };
        memcpy(&Owner->CurrentState.weapon_state.texture, Weapons[EquippedItemIdx]->WeaponData.texture, sizeof(Weapons[EquippedItemIdx]->WeaponData.texture));
    } else if (Owner != nullptr)
    {
        char texture[32];
        texture[0] = '\0';
        Owner->CurrentState.weapon_state = {
            NONE,
            {},
            -1,
            false
        };
        memcpy(&Owner->CurrentState.weapon_state.texture, texture, sizeof(texture));
    }
}

WeaponData Inventory::GetWeaponData(int Idx)
{
    WeaponData defaultReturn = {"", NONE, 0, 0, 0, 0, 0, 0};
    if (Idx < 0 || Idx >= INVENTORY_SIZE)
        return defaultReturn;
    if (Weapons[Idx] != nullptr)
        return Weapons[Idx]->WeaponData;
    return defaultReturn;
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
