//
// Created by lalit on 5/10/2026.
//

#include "Game.h"

Game::Game()
{
    LocalPlayer = Player(0, 0, 350.0f, this);
    GameCamera = BurstCamera(this);

    GameClient = Client(this);
    GameResources = Resources(this);

    GameResources.Load();
}

Game::~Game()
{
}

void Game::Connect(string IPAddress, int Port)
{
    LocalPlayer = Player(0, 0, 350.0f, this);
    GameClient.Connect(IPAddress, Port);
}

void Game::Disconnect()
{
    GameClient.Disconnect();
}

void Game::Update()
{
    LocalPlayer.MovePlayer(GameClient.GetServerTime());

    GameCamera.Start();

    for (auto &[PlayerID, Player] : GameClient.GetPlayers())
    {
        Player.SmoothPlayerState(GameClient.GetServerTime(), 0.25f);
        Player.Update();
    }

    LocalPlayer.Update();

    GameCamera.Update();
    GameCamera.Stop();

    GameClient.Update();
    GameClient.UpdateState(LocalPlayer.CurrentState);
}

void Game::Quit()
{
    Disconnect();
    GameResources.Unload();
}
