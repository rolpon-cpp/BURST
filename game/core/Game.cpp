//
// Created by  on 5/10/2026.
//

#include "Game.h"
#include <ostream>
#include "../world/WorldMap.h"
#include "../../network/Utils.h"

Game::Game()
{
    IsClient = false;
    MainMap = WorldMap(this);
    DeltaTime = 0.0f;
    LastTime = this->GetLocalTime();
    next_bullet_id = 1;
}

Game::~Game()
{
}

double Game::GetLocalTime()
{
    return GetTimeUtils();
}

double Game::GetServerTime()
{
    return GetTimeUtils();
}

double Game::GetDeltaTime()
{
    return DeltaTime;
}

void Game::AddBullet(Bullet b)
{
    if (!IsClient)
    {
        b.MyBulletData.id = next_bullet_id;
        b.MyBulletData.timestamp = GetServerTime();
        Bullets[next_bullet_id] = b;
    } else
    {
        bool found = false;

        for (auto &[name,value] : Bullets)
        {
            if (name == b.MyBulletData.client_id)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            b.MyBulletData.client_id = next_bullet_id;
            Bullets[b.MyBulletData.client_id] = b;
        }
    }
    next_bullet_id += 1;
}

void Game::AddBullet(BulletData bd)
{
    Bullet b = Bullet(this, bd);
    AddBullet(b);
}

void Game::Start(string IPAddress, int Port)
{
}

void Game::Stop()
{
    Bullets.clear();
    MainMap.ClearMap();
}

void Game::Update()
{
    DeltaTime = this->GetLocalTime() - LastTime;
    LastTime = this->GetLocalTime();

    std::erase_if(Bullets, [this](pair<uint64_t,Bullet> p)
    {
        bool ShouldDelete = p.second.MarkedForDeletion;
        if (ShouldDelete && !IsClient)
        {
            GameServer* server = (GameServer*)this;
            BulletDespawn d{p.first};
            server->MainServer.SendPacketToAll(BULLET_DESPAWN,&d,sizeof(d));
        }
        return ShouldDelete;
    });
    for (auto &[id,b] : Bullets)
        b.Update();
}

void Game::Quit()
{
    Stop();
}
