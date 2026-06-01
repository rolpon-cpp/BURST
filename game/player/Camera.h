//
// Created by lalit on 5/10/2026.
//

#ifndef BURST_CAMERA_H
#define BURST_CAMERA_H

#include "../../game_libs.h"

class Game;
class GameClient;

class BurstCamera
{

    int CameraShakes;
    double NextCameraShakeOffsetChange;
    Vector2 CameraShakeOffset;

public:

    GameClient *game;
    Camera2D RaylibCamera;

    BurstCamera();
    BurstCamera(GameClient *game);
    ~BurstCamera();

    Vector2 GetWorldMousePos();
    Vector2 GetCameraPos();

    void ShakeCamera(float Intensity);

    void Start();
    void Stop();
    void Update();
};


#endif //BURST_CAMERA_H