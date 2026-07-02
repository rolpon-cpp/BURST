#ifndef BURST_ADAPTIVEMUSIC_H
#define BURST_ADAPTIVEMUSIC_H

#include "raylib.h"

class Player;
class GameClient;

class AdaptiveMusic
{
    public:
    Player* Owner;
    GameClient* game;

    float FrameStressLevel;
    float StressLevel;
    double LastSwitchedMusic;

    int CurrentPlayingLayer;

    AdaptiveMusic();
    AdaptiveMusic(Player* Owner);
    ~AdaptiveMusic();

    void Update();
};


#endif //BURST_ADAPTIVEMUSIC_H