//
// Created by lalit on 5/10/2026.
//

#ifndef BURST_CAMERA_H
#define BURST_CAMERA_H

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include "raylib.h"

class Game;

class BurstCamera
{
public:
    Game *game;
    Camera2D RaylibCamera;

    BurstCamera();
    BurstCamera(Game *game);
    ~BurstCamera();

    void Start();
    void Stop();
    void Update();
};


#endif //BURST_CAMERA_H