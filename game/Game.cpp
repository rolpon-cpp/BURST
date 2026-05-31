//
// Created by lalit on 5/10/2026.
//

#include "Game.h"

Game::Game()
{
    MainMap = Map(this);
}

Game::~Game()
{
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
}

void Game::Quit()
{
    Stop();
}

GameClient::GameClient() : Game()
{
    IsClient = true;

    MainPlayer = Player(0, 0, 350.0f, this);
    MainCamera = BurstCamera(this);

    MainClient = Client(this);
    MainResources = Resources(this);

    MainUI = UI(this);

    MainResources.Load();
}

void GameClient::Start(string IPAddress, int Port)
{
    Game::Start(IPAddress, Port);
    MainPlayer = Player((WORLD_CHUNK_SIZE * CHUNK_SIZE * TILE_SIZE) / 2 - 18.0f, (WORLD_CHUNK_SIZE * CHUNK_SIZE * TILE_SIZE) / 2 - 18.0f, 350.0f, this);
    MainClient.Connect(IPAddress, Port);
}

void GameClient::Stop()
{
    Game::Stop();
    MainUI.Stop();
    MainClient.Disconnect();
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
    MainUI.Stop();
    Stop();
    MainResources.Unload();
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
    MainServer.StartServer(IPAddress, Port);
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
}

void GameServer::Quit()
{
    Game::Quit();
}
