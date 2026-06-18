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
    Stop();
    Game::Quit();
}

GameServer::~GameServer()
{
}