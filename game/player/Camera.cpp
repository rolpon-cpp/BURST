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

void BurstCamera::ShakeCamera(float Intensity)
{
    CameraShakes = max(min(Intensity / 0.125f, 1), 20);
    CameraShakeOffset = {(float)GetRandomValue(-70 * Intensity, 70 * Intensity), (float)GetRandomValue(-70 * Intensity, 70 * Intensity)};
    NextCameraShakeOffsetChange = game->MainClient.GetServerTime() + 0.01f;
}

void BurstCamera::Update()
{
    float Scroll = GetMouseWheelMove() * game->GetDeltaTime() * 10.0f;
    RaylibCamera.zoom += Scroll;
    RaylibCamera.zoom = min(max(RaylibCamera.zoom, 0.25f), 2.0f);
    if (CameraShakes > 0 && game->MainClient.GetServerTime() >= NextCameraShakeOffsetChange)
    {
        CameraShakeOffset = {(float)GetRandomValue(-50, 50), (float)GetRandomValue(-50, 50)};

        RaylibCamera.offset = {GetRenderWidth() / 2.0f + CameraShakeOffset.x, GetRenderHeight() / 2.0f + CameraShakeOffset.y};

        NextCameraShakeOffsetChange = game->MainClient.GetServerTime() + 0.01f;
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
