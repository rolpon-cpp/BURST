//
// Created by lalit on 6/13/2026.
//

#ifndef BURST_ANIMATION_H
#define BURST_ANIMATION_H
#include <cstdint>

#include "../Sounds.h"
#include "Particles.h"

class GameClient;

enum AnimationType
{
    ParticleAnimationEvent, SoundAnimationEvent, ParticleAndSoundAnimationEvent, NoneAnimationEvent
};

#pragma pack(push, 1)
struct AnimationEvent
{
    AnimationType type = NoneAnimationEvent;

    bool use_position = true;
    Vector2 position{0, 0};
    int32_t player_id = -1;

    ParticleEffect particle_effect{};
    SoundEffect sound_effect{};
};
#pragma pack(pop)

class Animator
{
public:
    GameClient* game;
    std::vector<AnimationEvent> AnimationEvents;
    Animator();
    Animator(GameClient* game);
    ~Animator();
    void Animate(AnimationEvent event);
    void Update();
    void Clear();
    void Quit();
};


#endif //BURST_ANIMATION_H