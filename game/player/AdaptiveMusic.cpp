#include "AdaptiveMusic.h"

#include <iostream>

#include "Player.h"
#include "../core/Game.h"
#include "raymath.h"

AdaptiveMusic::AdaptiveMusic()
{
}

AdaptiveMusic::AdaptiveMusic(Player* Owner)
{
    this->Owner = Owner;
    this->game = (GameClient*)Owner->game;

    this->LastSwitchedMusic = 0;
    this->FrameStressLevel = 0.0f;
    this->StressLevel = 0.0f;

    this->CurrentPlayingLayer = -1;
}

AdaptiveMusic::~AdaptiveMusic()
{
}

void AdaptiveMusic::Update()
{
    FrameStressLevel = 0.0f;
    if (Owner->CurrentState.health <= 50.0f)
    {
        FrameStressLevel += 0.5f;
        cout << "im dying " << Owner->CurrentState.health << "\n";
    }

    for (auto &[id,bullet] : game->Bullets)
    {
        float dist = Vector2Distance(Owner->CurrentState.position, bullet.CurrentPosition);
        if (dist <= 250)
            FrameStressLevel += 0.05f;
    }

    StressLevel = std::lerp(StressLevel, FrameStressLevel, 2.5f * game->GetDeltaTime());

    std::cout << StressLevel << " " << FrameStressLevel << "\n";

    int Layer = round(StressLevel / 0.25f);
    Layer += 1;

    if (Layer > 4)
        Layer = 4;
    if (Layer <= 0)
        Layer = 1;

    if (game->GetLocalTime() - LastSwitchedMusic >= 1.0f && CurrentPlayingLayer != Layer)
    {
        game->MainSounds.StopGameMusic("danger_layer"+to_string(CurrentPlayingLayer)+"_music", true);
        game->MainSounds.PlayGameMusic("danger_layer"+to_string(Layer)+"_music", true);
        CurrentPlayingLayer = Layer;
        LastSwitchedMusic = game->GetLocalTime();
    }
}
