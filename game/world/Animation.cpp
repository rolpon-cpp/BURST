//
// Created by lalit on 6/13/2026.
//

#include "Animation.h"

#include <iostream>

#include "../Game.h"

Animator::Animator()
{
}

Animator::Animator(GameClient* game)
{
    this->game = game;
}

Animator::~Animator()
{
}

void Animator::Animate(AnimationEvent event)
{
    if (event.type == NoneAnimationEvent)
        return;
    Vector2 p = event.position;
    if (!event.use_position && game->MainClient.Players.contains(event.player_id))
        p = game->MainClient.Players[event.player_id].GetCenter();
    if (!event.use_position && event.player_id == -1)
        p = game->MainPlayer.GetCenter();
    if (event.type == SoundAnimationEvent || event.type == ParticleAndSoundAnimationEvent)
    {
        event.sound_effect.set_properties(Vector2Distance(p, game->MainPlayer.GetCenter()));
        game->MainSounds.PlayGameSound(event.sound_effect);
    }
    if (event.type == ParticleAnimationEvent || event.type == ParticleAndSoundAnimationEvent)
    {
        event.particle_effect.impact = p;
        if (event.particle_effect.amount > 40)
            event.particle_effect.amount = 40;
        game->MainParticles.CreateParticles(event.particle_effect);
    }
}

void Animator::Update()
{
}

void Animator::Clear()
{
    AnimationEvents.clear();
}

void Animator::Quit()
{
    Clear();
}
