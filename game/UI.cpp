//
// Created by lalit on 5/19/2026.
//

#include "UI.h"
#include "Game.h"
#include "../network/Utils.h"

UI::UI()
{
}

UI::~UI()
{
    Stop();
}

UI::UI(GameClient* game)
{
    this->game = game;
    Stop();
}

void UI::Update()
{
    DrawText((to_string(static_cast<int>(round(game->MainClient.Ping * 1000.0f))) + "ms ping").c_str(), 15, 15, 20, BLACK);
    DrawText((to_string((int)round(GetFrameTime() * 1000.0f)) + "ms frame time").c_str(), 15, 35, 20, BLACK);

    float W = 250;
    float H = 56;
    Rectangle REC = {15, GetRenderHeight() - 15 - H, W, H};
    DrawRectangleRounded(REC, 0.2f, 5, ColorAlpha(BLUE, 0.5f));

    string FINAL_STR = "DASH " + to_string((int)max(
    (int) (((int)((1.0f - (GetTimeUtils() - game->MainPlayer.LastDashed)) * 10000.0f))/10.0f),
    0.0f
        )) + "ms";
    float TX_SIZE = MeasureText(FINAL_STR.c_str(), H - 24.0f);

    DrawText(FINAL_STR.c_str(), REC.x + REC.width / 2 - TX_SIZE / 2, REC.y + REC.height / 2 - (H - 24.0f) / 2, H - 24.0f, (GetTimeUtils() - game->MainPlayer.LastDashed >= 1.0f ? GREEN : ColorBrightness(BLUE, -0.3f)));

}

void UI::Stop()
{
}

void UI::Quit()
{
}
