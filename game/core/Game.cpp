//
// Created by  on 5/10/2026.
//

#include "Game.h"
#include <iostream>
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
    }
    Bullets[IsClient ? b.MyBulletData.id : next_bullet_id] = b;
    if (!IsClient)
        next_bullet_id += 1;
}

void Game::AddBullet(BulletData bd)
{
    if (!IsClient)
    {
        bd.id = next_bullet_id;
        bd.timestamp = GetServerTime();
    }
    Bullets[IsClient ? bd.id : next_bullet_id] = Bullet(this, bd);
    if (!IsClient)
        next_bullet_id += 1;
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
