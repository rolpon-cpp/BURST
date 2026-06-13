//
// Created by lalit on 5/10/2026.
//

#include "Game.h"
#include <iostream>
#include <ostream>
#include "world/WorldMap.h"
#include "../network/Utils.h"

Game::Game()
{
    IsClient = false;
    MainMap = WorldMap(this);
    DeltaTime = 0.0f;
    LastTime = this->GetTime();
}

Game::~Game()
{
}

double Game::GetTime()
{
    if (IsClient)
        return static_cast<GameClient*>(this)->MainClient.GetServerTime();
    return GetTimeUtils();
}

double Game::GetDeltaTime()
{
    return DeltaTime;
}

void Game::Start(string IPAddress, int Port)
{
}

void Game::Stop()
{
    MainMap.ClearMap();
}

void Game::Update()
{
    DeltaTime = this->GetTime() - LastTime;
    LastTime = this->GetTime();
}

void Game::Quit()
{
    Stop();
}

GameClient::GameClient() : Game()
{
    IsClient = true;

    MainCamera = BurstCamera(this);
    MainPlayer = Player(0, 0, 350.0f, this);
    MainClient = Client(this);
    MainResources = Resources(this);
    MainSounds = Sounds(this);
    MainUI = UI(this);

    MainResources.Load();
}

void GameClient::Start(string IPAddress, int Port)
{
    Game::Start(IPAddress, Port);
    MainPlayer.Destroy();
    MainPlayer = Player((WORLD_CHUNK_SIZE * CHUNK_SIZE * TILE_SIZE) / 2 - 18.0f, (WORLD_CHUNK_SIZE * CHUNK_SIZE * TILE_SIZE) / 2 - 18.0f, 350.0f, this);
    MainClient.Connect(IPAddress, Port);
}

void GameClient::Stop()
{
    Game::Stop();
    MainUI.Stop();
    MainClient.Disconnect();
    MainSounds.Clear();
}

void GameClient::Update()
{
    Game::Update();

    MainCamera.Start();

    MainMap.Update();

    for (auto &[PlayerID, Player] : MainClient.GetPlayers())
    {
        //max((float) MainClient.Ping * 2.1f, 1.0f / 40.f)
        Player.SmoothPlayerState(0.05f);
        Player.Update();
    }

    MainPlayer.Update();

    MainCamera.Update();
    MainCamera.Stop();

    MainUI.Update();

    MainClient.Update();
}

void GameClient::Quit()
{
    Game::Quit();
    Stop();
    MainUI.Stop();
    MainResources.Unload();
    MainSounds.Quit();
    MainPlayer.Destroy();
}

GameServer::GameServer() : Game()
{
    IsClient = false;
    MainServer = Server(this);
}

void GameServer::Start(string IPAddress, int Port)
{
    Game::Start(IPAddress, Port);
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

GameClient::~GameClient()
{
}

GameServer::~GameServer()
{
}
