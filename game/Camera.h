//
// Created by lalit on 5/10/2026.
//

#ifndef BURST_CAMERA_H
#define BURST_CAMERA_H

#include "../game_libs.h"

class Game;
class GameClient;

class BurstCamera
{
public:
    GameClient *game;
    Camera2D RaylibCamera;

    BurstCamera();
    BurstCamera(GameClient *game);
    ~BurstCamera();

    void Start();
    void Stop();
    void Update();
};


#endif //BURST_CAMERA_H