//
// Created by lalit on 5/10/2026.
//

#ifndef BURST_GAME_H
#define BURST_GAME_H
#include "Resources.h"
#include "../network/client/Client.h"
#include "UI.h"
#include "player/Camera.h"
#include "world/Map.h"
#include "../network/server/Server.h"

class Game
{
public:
    bool IsClient;
    Map MainMap;
    double LastTime;
    double DeltaTime;

    Game();
    ~Game();

    void Start(string IPAddress = "127.0.0.1", int Port = 5000);
    double GetTime();
    double GetDeltaTime();
    void Stop();
    void Update();
    void Quit();
};

class GameClient : public Game
{
public:
    Player MainPlayer;
    BurstCamera MainCamera;
    Client MainClient;
    Resources MainResources;
    UI MainUI;
    GameClient();
    void Start(string IPAddress = "127.0.0.1", int Port = 5000);
    void Stop();
    void Update();
    void Quit();
};

class GameServer : public Game
{
public:
    Server MainServer;
    GameServer();
    void Start(string IPAddress = "127.0.0.1", int Port = 5000);
    void Stop();
    void Update();
    void Quit();
};

#endif //BURST_GAME_H