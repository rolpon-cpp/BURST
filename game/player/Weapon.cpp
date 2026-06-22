//
// Created by  on 5/20/2026.
//

#include "Weapon.h"

#include <iostream>

#include "../core/Game.h"

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
    if (this->CooldownState > 0.0f)
        this->CooldownState -= inventory->game->GetDeltaTime();
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
    /*
    cout << "ATTACK FUNC ACTIVATED! Info: COOLDOWN: " << CooldownState << ", AMMO: " << Ammo << ", ATTACK INFO: ORIGIN: (" << attackInfo.origin.x <<
        ", " << attackInfo.origin.y << "), TARGET: (" << attackInfo.target.x << ", " << attackInfo.target.y << "), INVENTORY IDX: " <<
            attackInfo.inventoryIdx << ", TIMESTAMP: " << attackInfo.timestamp << "\n";
            */
    if (!this->CanAttack())
    {
        return;
    }
    Weapon::Attack(attackInfo);

    AnimationEvent event;
    event.type = SoundAnimationEvent;
    event.use_position = false;
    event.player_id = this->inventory->Owner->PlayerID;
    event.position = this->inventory->Owner->GetCenter();
    event.sound_effect =
    {
        "",
        1.0f,
        1.0f,
    };
    memcpy(&event.sound_effect,this->WeaponData.sound,32);

    if (inventory->game->IsClient)
    {
        GameClient* game_client = (GameClient*) inventory->game;
        game_client->MainClient.AttackWithWeapon(attackInfo);
        game_client->MainAnimator.Animate(event);
    } else
    {
        GameServer* game_server = (GameServer*) inventory->game;

        game_server->MainServer.SendPacketToAll(ANIMATION, &event, sizeof(event), {this->inventory->Owner->PlayerID});

        for (int i = 0; i < WeaponData.shots; i++)
        {
            float Angle = 180.0f - Vector2LineAngle(attackInfo.origin, attackInfo.target) * RAD2DEG;
            if (WeaponData.angle_range > 0)
            {
                Angle -= WeaponData.angle_range / 2.0f;
                Angle += (WeaponData.angle_range / WeaponData.shots) / 2.0f;
            }

            game_server->AddBullet(BulletData{
                0, this->inventory->Owner->PlayerID,
                inventory->Owner->GetPlayerState(attackInfo.timestamp).GetCenter(),
                Vector2Normalize({-cos(Angle * (2 * PI / 360)) * 100.0f,-sin(Angle * (2 * PI / 360)) * 100.0f}),
                350.0f,
                10.0f,
                WeaponData.damage,
                game_server->GetServerTime()
            });
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
    this->game = game;
    this->Owner = MyPlayerOwner;
    this->Weapons[0] = nullptr;
    this->Weapons[1] = nullptr;
    this->Weapons[2] = nullptr;
    this->EquippedItemIdx = -1;
    this->ReloadTime = 0;
    this->IsReloading = false;
}

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
}

bool Inventory::IsHoldingItem()
{
    return EquippedItemIdx != -1 && EquippedItemIdx >= 0 && EquippedItemIdx < INVENTORY_SIZE;
}

void Inventory::SetItem(WeaponData newWeaponData, int Idx)
{
    std::shared_ptr<Weapon> wep = nullptr;

    if (newWeaponData.type == NONE)
    {
        DropItem(Idx);
        return;
    }

    if (Weapons[Idx] != nullptr && Weapons[Idx]->WeaponData == newWeaponData)
        return;

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

void Inventory::GiveItem(WeaponData newWeaponData)
{
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        if (Weapons[i] == nullptr)
        {
            SetItem(newWeaponData, i);
            break;
        }
    }
}

void Inventory::GiveItem(std::shared_ptr<Weapon> newWeapon)
{
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        if (Weapons[i] == nullptr)
        {
            SetItem(newWeapon,i);
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
    else
        UnequipItem();
}

void Inventory::UnequipItem()
{
    this->EquippedItemIdx = -1;
    this->ReloadTime = 0;
    this->IsReloading = false;
}

void Inventory::Attack(int Idx, Vector2 Target)
{
    if (Idx < 0 || Idx >= INVENTORY_SIZE)
        return;
    WeaponAttack attackInfo = {
        Owner->GetCenter(),
        Target,
        Idx,
        game->GetServerTime()
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
    if (IsReloading)
        return;
    if (Weapons[attackInfo.inventoryIdx] != nullptr)
    {
        Weapons[attackInfo.inventoryIdx]->Attack(attackInfo);
    }
}

void Inventory::Reload()
{
    if (!IsHoldingItem())
        return;
    if (game->IsClient)
        ((GameClient*)game)->MainClient.ReloadWeapon();
    this->ReloadTime = 0.5f;
    this->IsReloading = true;
    cout << "RELOAD\n";
}

void Inventory::Update()
{
    if (IsReloading)
        cout << "IS reloading, " << ReloadTime << "\n";
    if (!IsHoldingItem())
    {
        IsReloading = false;
        ReloadTime = 0.0f;
    }
    if (game != nullptr && IsReloading && ReloadTime > 0.0f)
    {
        this->ReloadTime -= game->GetDeltaTime();
        cout << "subtracting time reload\n";
    }
    if (IsReloading && ReloadTime <= 0.0f && IsHoldingItem() && Weapons[EquippedItemIdx]->WeaponData.type == PROJECTILE)
    {
        ((ProjectileWeapon*)Weapons[EquippedItemIdx].get())->Ammo = Weapons[EquippedItemIdx]->WeaponData.ammo;
        this->ReloadTime = 0;
        this->IsReloading = false;
        cout << "finish reload\n";
    }

    SetCharacterWeaponState();

    if (game != nullptr && game->IsClient)
    {
        if (Owner->IsLocalPlayer())
        {
            if (Owner->CurrentState.health > 0)
            {
                if (!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT))
                {
                    if (IsKeyPressed(KEY_ONE))
                        EquipItem(0);
                    if (IsKeyPressed(KEY_TWO))
                        EquipItem(1);
                    if (IsKeyPressed(KEY_THREE))
                        EquipItem(2);
                }
                if (IsKeyPressed(KEY_R) && IsHoldingItem())
                    Reload();
            } else
            {
                UnequipItem();
            }
        }

        if (IsMouseButtonPressed(0) && Owner->IsLocalPlayer() && Owner->CurrentState.health > 0 && !IsReloading)
        {
            Attack(((GameClient*)game)->MainCamera.GetWorldMousePos());
        }

        std::string c = string(reinterpret_cast<char*>(Owner->CurrentState.weapon_state.texture));

        if (!c.empty())
        {
            GameClient* game_c = (GameClient*)game;

            WeaponRenderRot = LerpAngle(WeaponRenderRot, Owner->CurrentState.rotation, 24.0f * game->GetDeltaTime());

            Vector2 offset = {
                cosf(WeaponRenderRot * DEG2RAD) * 100.0f,
                sinf(WeaponRenderRot * DEG2RAD) * 100.0f
            };

            Texture2D& g = game_c->MainResources.GetTexture(c);
            DrawTexturePro(g, {0, 0, (float) g.width, (float) g.height * (abs(WeaponRenderRot) > 90.0f ? -1.0f : 1.0f)}, {
                Owner->GetCenter().x - offset.x, Owner->GetCenter().y - offset.y,
                (float)g.width * 3.0f, (float)g.height * 3.0f,
            }, {g.width * 1.5f, g.height * 1.5f}, WeaponRenderRot, WHITE);
        }
    }

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
    WeaponData defaultReturn = {"", "", NONE, 0, 0, 0, 0, 0, 0, 0};
    if (Idx < 0 || Idx >= INVENTORY_SIZE)
        return defaultReturn;
    if (Weapons[Idx] != nullptr)
        return Weapons[Idx]->WeaponData;
    return defaultReturn;
}

void Inventory::Destroy()
{
    this->ReloadTime = 0;
    this->IsReloading = false;
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        if (Weapons[i] != nullptr)
            Weapons[i].reset();
    }
}
