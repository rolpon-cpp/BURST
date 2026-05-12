//
// Created by lalit on 5/10/2026.
//

#ifndef BURST_GAME_H
#define BURST_GAME_H
#include "Resources.h"
#include "../network/client/Client.h"

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include "Camera.h"
#include "raylib.h"

class Game
{
public:
    // Player & Camera
    Player LocalPlayer;
    BurstCamera GameCamera;

    // Network
    Client GameClient;

    // Managers
    Resources GameResources;

    Game();
    ~Game();

    void Connect(string IPAddress, int Port);
    void Disconnect();
    void Update();
    void Quit();
};


#endif //BURST_GAME_H