//
// Created by lalit on 5/10/2026.
//

#include "Camera.h"

#include "../Game.h"

BurstCamera::BurstCamera()
{
}

BurstCamera::BurstCamera(GameClient* game)
{
    this->game = game;
    this->RaylibCamera = { GetRenderWidth() / 2.0f, GetRenderHeight() / 2.0f, 0, 0, 0, 1.0f};

    this->CameraShakes = 0;
    this->NextCameraShakeOffsetChange = 0.0f;
    this->CameraShakeOffset = {0, 0};
}

BurstCamera::~BurstCamera()
{
}

Vector2 BurstCamera::GetWorldMousePos()
{
    return GetScreenToWorld2D(GetMousePosition(), RaylibCamera);
}

Vector2 BurstCamera::GetCameraPos()
{
    return RaylibCamera.target;
}

void BurstCamera::Update()
{
    if (CameraShakes > 0 && game->MainClient.GetServerTime() >= NextCameraShakeOffsetChange)
    {
        if (CameraShakes == 1)
            CameraShakeOffset = {0, 0};
        else
            CameraShakeOffset = {(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)};

        RaylibCamera.offset = {GetRenderWidth() / 2.0f + CameraShakeOffset.x, GetRenderHeight() / 2.0f + CameraShakeOffset.y};

        CameraShakes--;
    }
    if (game->MainClient.Connected)
    {
        this->RaylibCamera.target = Vector2Lerp(this->RaylibCamera.target,
            game->MainPlayer.GetCenter(), 6.5f * game->GetDeltaTime());
    }
}

void BurstCamera::Start()
{
    BeginMode2D(RaylibCamera);
}

void BurstCamera::Stop()
{
    EndMode2D();
}
