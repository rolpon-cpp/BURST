//
// Created by  on 5/10/2026.
//

#include "Camera.h"

#include "../core/Game.h"

BurstCamera::BurstCamera()
{
}

BurstCamera::BurstCamera(GameClient* game)
{
    this->game = game;
    this->RaylibCamera = { GetRenderWidth() / 2.0f, GetRenderHeight() / 2.0f, 0, 0, 0, 1.0f};
    this->IsZoomingCamera = false;
    this->CameraShakes = 0;
    this->NextCameraShakeOffsetChange = 0.0f;
    this->CameraPos = {0, 0};
    this->CameraZoomTime = 0.0f;
    this->CameraZoom = 1.0f;
    this->CameraZoomTarget = 0.0f;
    this->CameraShakeIntensity = 0;
    this->CameraShakeOffset = {0, 0};
}

BurstCamera::~BurstCamera()
{
}

void BurstCamera::ZoomCamera(float Zoom, float Time)
{
    IsZoomingCamera = true;
    CameraZoomTarget = Zoom;
    CameraZoomTime = Time;
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
    CameraShakeIntensity = Intensity;
}

void BurstCamera::Background()
{
    float BackgroundDepth = 3;
    float BackgroundGridSize = 36;
    float ParallaxCamX = CameraPos.x / BackgroundDepth;
    float ParallaxCamY = CameraPos.y / BackgroundDepth;
    for (int i = -1; i < round(GetRenderHeight() / BackgroundGridSize) + 1; i++)
    {
        int y = (int)(ParallaxCamY / BackgroundGridSize);
        DrawLineEx({CameraPos.x - GetRenderWidth()/2.0f, ((y + i) * BackgroundGridSize) - ParallaxCamY + CameraPos.y - GetRenderHeight()/2.0f}, {
                       (float)GetRenderWidth() + CameraPos.x - GetRenderWidth()/2.0f,
                       ((y + i) * BackgroundGridSize) - ParallaxCamY + CameraPos.y - GetRenderHeight()/2.0f
                   }, 7, ColorBrightness(WHITE, -0.5f));
    }

    for (int i = -1; i < round(GetRenderWidth() / BackgroundGridSize) + 1; i++)
    {
        int x = (int)(ParallaxCamX / BackgroundGridSize);
        DrawLineEx({((x + i) * BackgroundGridSize) - ParallaxCamX + CameraPos.x - GetRenderWidth()/2.0f, CameraPos.y - GetRenderHeight()/2.0f}, {
                       ((x + i) * BackgroundGridSize) - ParallaxCamX + CameraPos.x - GetRenderWidth()/2.0f,
                       (float)GetRenderHeight() + CameraPos.y - GetRenderHeight()/2.0f
                   }, 7, ColorBrightness(WHITE, -0.5f));
        DrawLineEx({((x + i) * BackgroundGridSize) - ParallaxCamX + CameraPos.x - GetRenderWidth()/2.0f, CameraPos.y - GetRenderHeight()/2.0f}, {
                       ((x + i) * BackgroundGridSize) - ParallaxCamX + CameraPos.x - GetRenderWidth()/2.0f,
                       (float)GetRenderHeight() + CameraPos.y - GetRenderHeight()/2.0f
                   }, 3, ColorAlpha(WHITE, 0.5f));
    }

    for (int i = -1; i < round(GetRenderHeight() / BackgroundGridSize) + 1; i++)
    {
        int y = (int)(ParallaxCamY / BackgroundGridSize);
        DrawLineEx({CameraPos.x - GetRenderWidth()/2.0f, ((y + i) * BackgroundGridSize) - ParallaxCamY + CameraPos.y - GetRenderHeight()/2.0f}, {
                       (float)GetRenderWidth() + CameraPos.x - GetRenderWidth()/2.0f,
                       ((y + i) * BackgroundGridSize) - ParallaxCamY + CameraPos.y - GetRenderHeight()/2.0f
                   }, 3, ColorAlpha(WHITE, 0.5f));
    }
}

void BurstCamera::Update()
{

    if (CameraShakes > 0 && game->MainClient.GetServerTime() >= NextCameraShakeOffsetChange)
    {
        CameraShakeOffset = {(float)GetRandomValue(-70 * CameraShakeIntensity, 70 * CameraShakeIntensity), (float)GetRandomValue(-70 * CameraShakeIntensity, 70 * CameraShakeIntensity)};

        NextCameraShakeOffsetChange = game->MainClient.GetServerTime() + 0.005f;
        CameraShakes--;
    }
    if (CameraShakes <= 0)
        CameraShakeOffset = {0,0};
    if (game->MainClient.Connected)
    {
        CameraPos = Vector2Lerp(CameraPos,
            game->MainPlayer.GetCenter(), 6.5f * game->GetDeltaTime());
        RaylibCamera.target = CameraPos + CameraShakeOffset;
    }

    IsZoomingCamera = CameraZoomTime > 0.0f;

    if (!IsZoomingCamera)
        CameraZoom = lerp(CameraZoom, 1.0f, 5.0f * game->GetDeltaTime());
    else
    {
        CameraZoom = lerp(CameraZoom, CameraZoomTarget, 10.0f * game->GetDeltaTime());
        CameraZoomTime -= game->GetDeltaTime();
    }

    RaylibCamera.zoom = CameraZoom * (GetScreenWidth() / 1280.0f);
}

void BurstCamera::Start()
{
    BeginMode2D(RaylibCamera);
    Background();
}

void BurstCamera::Stop()
{
    EndMode2D();
}
