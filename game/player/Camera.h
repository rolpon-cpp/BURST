//
// Created by  on 5/10/2026.
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
    bool IsZoomingCamera = false;

    Camera2D RaylibCamera;

public:

    GameClient *game;

    Vector2 CameraPos;
    float CameraZoom;

    BurstCamera();
    BurstCamera(GameClient *game);
    ~BurstCamera();

    Vector2 GetWorldMousePos();
    Vector2 GetCameraPos();

    void ShakeCamera(float Intensity);
    void ZoomCamera(float Zoom);

    void Start();
    void Stop();
    void Update();
};


#endif //BURST_CAMERA_H