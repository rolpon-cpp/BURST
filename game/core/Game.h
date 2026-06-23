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
#include "../player/Bullet.h"
#include "../res/ServerResources.h"
#include "../world/Animation.h"
#include "../world/Particles.h"

class Game
{
public:
    bool IsClient;

    WorldMap MainMap;
    std::unordered_map<uint64_t, Bullet> Bullets;
    uint64_t next_bullet_id = -1;

    double LastTime;
    double DeltaTime;

    Game();
    virtual ~Game();

    virtual void Start(string IPAddress = "127.0.0.1", int Port = 5000);
    virtual void Stop();
    virtual void Update();
    virtual void Quit();

    virtual double GetLocalTime();
    virtual double GetServerTime();
    virtual double GetDeltaTime();

    virtual void AddBullet(Bullet b);
    virtual void AddBullet(BulletData bd);
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
    double GetServerTime() override;
    void Start(string IPAddress = "127.0.0.1", int Port = 5000, PlayerCustomizedItems CustomizedItems = {});
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
    void AddBullet(Bullet b);
    void AddBullet(BulletData bd);
};

#endif //BURST_GAME_H