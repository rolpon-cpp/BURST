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
        DrawText(c, UIRenderTexture.texture.width/2.0f - siz/2.0f, UIRenderTexture.texture.height * 0.25f, 40, RED);
        if (IsKeyPressed(KEY_R))
            game->MainClient.Respawn();
    }

    float DashBarRectWidth = 46.0f;
    float DashBarRectHeight = 250.0f;
    Rectangle DashRect = {UIRenderTexture.texture.width - DashBarRectWidth - 30.0f, UIRenderTexture.texture.height / 2.0f - DashBarRectHeight/2.0f, DashBarRectWidth,DashBarRectHeight};
    DrawRectangleRounded(
        DashRect,
        0.1f,
        2,
        ColorAlpha(BLACK, 0.5f)
        );

    DrawRectangleRounded(
        {DashRect.x + 5.0f, DashRect.y + 5.0f, DashRect.width - 10.0f, (DashRect.height * (float) min(game->GetLocalTime() - game->MainPlayer.LastMovementAttack, 1.0f)) - 10.0f},
        0.1f,
        2,
        BLUE
        );

    float InventorySlotRectSize = 64.0f;
    float InventorySlotMargin = 5.0f;

    float TotalSize = (InventorySlotRectSize * INVENTORY_SIZE) + ((INVENTORY_SIZE - 1) * InventorySlotMargin);

    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        float x = UIRenderTexture.texture.width / 2.0f - TotalSize / 2.0f;
        x += i * InventorySlotRectSize;
        x += i * InventorySlotMargin;

        Rectangle UISlotRect = {x, UIRenderTexture.texture.height - InventorySlotRectSize - (InventorySlotMargin * 3.0f),
            InventorySlotRectSize, InventorySlotRectSize};

        DrawRectangleRounded(UISlotRect,0.1f,2,
            ColorAlpha(BLACK, (i == game->MainPlayer.inventory.EquippedItemIdx ? 0.85f : 0.65f)));

        if (game->MainPlayer.inventory.Weapons[i])
        {
            Texture& tex = game->MainResources.GetTexture(string((char*)game->MainPlayer.inventory.Weapons[i]->WeaponData.texture));

            float TexWidth = tex.width;
            float TexHeight = tex.height;

            float BiggerSize = max(TexWidth, TexHeight);
            float Mul = InventorySlotRectSize / BiggerSize;
            TexWidth *= Mul;
            TexHeight *= Mul;

            DrawTexturePro(tex, {0,0,
                static_cast<float>(tex.width),static_cast<float>(tex.height)},
                {
                UISlotRect.x + UISlotRect.width/2.0f,
                UISlotRect.y + UISlotRect.height/2.0f,
                TexWidth,TexHeight
            },
            {TexWidth / 2.0f,TexHeight / 2.0f}, 45.0f,
            ColorAlpha(WHITE, (i == game->MainPlayer.inventory.EquippedItemIdx ? 1.0f : 0.75f)));
        }
    }

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
