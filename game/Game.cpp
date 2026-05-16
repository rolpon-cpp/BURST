//
// Created by lalit on 5/10/2026.
//

#include "Game.h"

Game::Game()
{
    LocalPlayer = Player(0, 0, 350.0f, this);
    GameCamera = BurstCamera(this);
    GameMap = Map();

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
    GameMap.ClearMap();
    GameClient.Disconnect();
}

void Game::Update()
{
    LocalPlayer.MovePlayer(GameClient.GetServerTime());

    GameCamera.Start();

    for (auto &[PlayerID, Player] : GameClient.GetPlayers())
    {
        Player.SmoothPlayerState(GameClient.GetServerTime(), 0.1f);
        Player.Update();
    }

    for (int x = 0; x < WORLD_CHUNK_SIZE; x++)
    {
        for (int y = 0; y < WORLD_CHUNK_SIZE; y++)
        {
            Chunk* c = GameMap.GetChunk(x, y);
            if (c == nullptr && !GameMap.ChunkIsMarked(x, y))
                GameMap.MarkChunk(x, y);
        }
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
