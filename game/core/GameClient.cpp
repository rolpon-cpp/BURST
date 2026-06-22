#include "Game.h"
#include <ostream>
#include "../world/WorldMap.h"

GameClient::GameClient() : Game()
{
    IsClient = true;

    MainCamera = BurstCamera(this);
    MainPlayer = Player(0, 0, 350.0f, this);
    MainClient = Client(this);
    MainResources = ClientResources(this);
    MainSounds = Sounds(this);
    MainAnimator = Animator(this);
    MainParticles = Particles(this);
    MainUI = UI(this);

    MainResources.Load();
}

GameClient::~GameClient()
{
}

double GameClient::GetServerTime()
{
    return MainClient.GetServerTime();
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
    MainParticles.Clear();
    MainAnimator.Clear();
    MainClient.Disconnect();
    MainSounds.Clear();
}

void GameClient::Update()
{
    MainCamera.Start();

    Game::Update();

    MainMap.Update();

    for (auto &[PlayerID, Player] : MainClient.GetPlayers())
    {
        //max((float) MainClient.Ping * 2.1f, 1.0f / 40.f)
        Player.SmoothPlayerState(0.05f);
        Player.Update();
    }
    MainPlayer.Update();

    MainAnimator.Update();
    MainParticles.Update();
    MainSounds.Update();

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
    MainParticles.Quit();
    MainAnimator.Quit();
    MainPlayer.Destroy();
}