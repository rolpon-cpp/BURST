//
// Created by lalit on 5/19/2026.
//

#ifndef BURST_UI_H
#define BURST_UI_H


class GameClient;

class UI
{
    public:
    GameClient* game;
    UI();
    ~UI();
    UI(GameClient* game);
    void Update();
    void Stop();
    void Quit();
};


#endif //BURST_UI_H