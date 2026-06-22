//
// Created by  on 6/16/2026.
//

#include "Game.h"
#include <ostream>
#include "../world/WorldMap.h"

GameServer::GameServer() : Game()
{
    IsClient = false;
    MainServer = Server(this);
    MainResources = ServerResources(this);

    MainResources.Load();
}

void GameServer::Start(int Port)
{
    Game::Start("", Port);
    MainMap.GenerateMap(GetRandomValue(1000,5000));
    MainServer.StartServer(Port);
}

void GameServer::Stop()
{
    Game::Stop();
    MainServer.StopServer();
}

void GameServer::Update()
{
    Game::Update();
    MainServer.UpdateServer();

    for (auto &[id, peer] : MainServer.Players)
    {
        Player* plr = (Player*)peer->data;
        plr->Update();
    }
}

void GameServer::Quit()
{
    MainResources.Unload();
    Stop();
    Game::Quit();
}

void GameServer::AddBullet(Bullet b)
{
    b.MyBulletData.id = next_bullet_id;
    b.MyBulletData.timestamp = GetServerTime();
    MainServer.SendPacketToAll(BULLET_SPAWN,&b.MyBulletData,sizeof(b));
    Game::AddBullet(b);
}

void GameServer::AddBullet(BulletData bd)
{
    bd.id = next_bullet_id;
    bd.timestamp = GetServerTime();
    MainServer.SendPacketToAll(BULLET_SPAWN,&bd,sizeof(bd));
    Game::AddBullet(bd);
}

GameServer::~GameServer()
{
}
