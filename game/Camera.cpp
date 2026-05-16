//
// Created by lalit on 5/10/2026.
//

#include "Camera.h"

#include "Game.h"

BurstCamera::BurstCamera()
{
}

BurstCamera::BurstCamera(GameClient* game)
{
    this->game = game;
    this->RaylibCamera = { GetRenderWidth() / 2.0f, GetRenderHeight() / 2.0f, 0, 0, 0, 1.0f};
}

BurstCamera::~BurstCamera()
{
}

void BurstCamera::Update()
{
    if (game->MainClient.Connected)
    {
        this->RaylibCamera.target = Vector2Lerp(this->RaylibCamera.target, game->MainPlayer.CurrentState.position, 10.0f * GetFrameTime());
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
