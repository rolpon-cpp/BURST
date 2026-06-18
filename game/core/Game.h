//
// Created by  on 5/10/2026.
//

#ifndef BURST_GAME_H
#define BURST_GAME_H
#include "../res/ClientResources.h"
#include "../world/Sounds.h"
#include "../../network/client/Client.h"
#include "UI.h"
#include "../player/Camera.h"
#include "../world/WorldMap.h"
#include "../../network/server/Server.h"
#include "../res/ServerResources.h"
#include "../world/Animation.h"
#include "../world/Particles.h"

class Game
{
public:
    bool IsClient;
    WorldMap MainMap;
    double LastTime;
    double DeltaTime;

    Game();
    virtual ~Game();

    virtual void Start(string IPAddress = "127.0.0.1", int Port = 5000);
    virtual double GetTime();
    virtual double GetDeltaTime();
    virtual void Stop();
    virtual void Update();
    virtual void Quit();
};

class GameClient : public Game
{
public:
    Player MainPlayer;
    Sounds MainSounds;
    BurstCamera MainCamera;
    Particles MainParticles;
    Animator MainAnimator;
    Client MainClient;
    ClientResources MainResources;
    UI MainUI;
    GameClient();
    virtual ~GameClient();
    void Start(string IPAddress = "127.0.0.1", int Port = 5000);
    void Stop();
    void Update();
    void Quit();
};

class GameServer : public Game
{
public:
    Server MainServer;
    ServerResources MainResources;
    GameServer();
    ~GameServer();
    void Start(int Port = 5000);
    void Stop();
    void Update();
    void Quit();
};

#endif //BURST_GAME_H