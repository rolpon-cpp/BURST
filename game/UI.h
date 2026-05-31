//
// Created by lalit on 5/19/2026.
//

#ifndef BURST_UI_H
#define BURST_UI_H
#include "../game_libs.h"

class GameClient;

class UI
{
    int VignetteTransparencyUniformLoc;
    public:
    GameClient* game;
    RenderTexture2D UIRenderTexture;
    UI();
    ~UI();
    UI(GameClient* game);
    void Update();
    void Stop();
    void Quit();
};


#endif //BURST_UI_H