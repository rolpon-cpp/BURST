//
// Created by  on 5/19/2026.
//

#include "UI.h"

#include <iostream>

#include "Game.h"
#include "../../network/Utils.h"

UI::UI()
{
}

UI::~UI()
{
    UIRenderTexture = LoadRenderTexture(1280, 720);
    VignetteTransparencyUniformLoc = -1;
    Stop();
}

UI::UI(GameClient* game)
{
    this->game = game;
    UIRenderTexture = LoadRenderTexture(1280, 720);
    VignetteTransparencyUniformLoc = -1;
    Stop();
}

void UI::Update()
{
    if (VignetteTransparencyUniformLoc == -1)
        VignetteTransparencyUniformLoc = GetShaderLocation(game->MainResources.GetShader("vignette"), "Transparency");

    BeginBlendMode(BLEND_ALPHA);
    BeginTextureMode(UIRenderTexture);
    ClearBackground(BLANK);

    DrawText((to_string(static_cast<int>(round(game->MainClient.Ping * 1000.0f))) + "ms ping").c_str(), 15, 15, 20, BLACK);
    DrawText((to_string((int)round(game->GetDeltaTime() * 1000.0f)) + "ms frame time").c_str(), 15, 35, 20, BLACK);

    if (game->MainPlayer.CurrentState.health <= 0.0f)
    {
        const char* c = "u ded :(, r to revive";
        float siz = MeasureText(c, 40.0f);
        DrawText(c, UIRenderTexture.texture.width/2.0f - siz/2.0f, UIRenderTexture.texture.height * 0.75f, 40, RED);
        if (IsKeyPressed(KEY_R))
            game->MainClient.Respawn();
    }

    float W = 250;
    float H = 56;
    Rectangle REC = {15, UIRenderTexture.texture.height - 15 - H, W, H};
    DrawRectangleRounded(REC, 0.2f, 5, ColorAlpha(BLUE, 0.5f));

    string FINAL_STR = "DASH " + to_string((int)max((int) (((int)((1.0f - (game->GetTime() - game->MainPlayer.LastDashed)) * 10000.0f))/10.0f), 0)) + "ms";
    float TX_SIZE = MeasureText(FINAL_STR.c_str(), H - 24.0f);

    DrawText(FINAL_STR.c_str(), REC.x + REC.width / 2 - TX_SIZE / 2, REC.y + REC.height / 2 - (H - 24.0f) / 2, H - 24.0f, (game->GetTime() - game->MainPlayer.LastDashed >= 1.0f ? GREEN : ColorBrightness(BLUE, -0.3f)));
    EndTextureMode();

    BeginShaderMode(game->MainResources.GetShader("vignette"));

    float VignetteTransparency = game->MainPlayer.DisplayHealth / 100.0f;
    VignetteTransparency = 1.0f - VignetteTransparency;
    SetShaderValue(game->MainResources.GetShader("vignette"), VignetteTransparencyUniformLoc, &VignetteTransparency, SHADER_UNIFORM_FLOAT);

    DrawTexturePro(UIRenderTexture.texture, {0, 0, 1280.0f, -720.0f}, {0, 0, (float)GetRenderWidth(), (float)GetRenderHeight()}, {0, 0}, 0, WHITE);
    EndShaderMode();
    EndBlendMode();
}

void UI::Stop()
{
}

void UI::Quit()
{
    UnloadRenderTexture(UIRenderTexture);
}
