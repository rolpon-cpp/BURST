//
// Created by lalit on 5/22/2026.
//

#ifndef BURST_PARTICLES_H
#define BURST_PARTICLES_H

#include <vector>

#include "../../game_libs.h"

class GameClient;

#pragma pack(push, 1)
struct ParticleEffect
{
    int amount = 0;
    Vector2 impact = {0, 0};
    Vector2 direction = {0, 0};
    float lifetime = 0;
    float velocity = 0;
    float velocity_slowdown = 0;
    float size = 0;
    Color color = BLANK;

    float angle_variety = 0;
    float impact_variety = 0;
    float size_variety = 0;
    float velocity_slowdown_variety = 0;
    float lifetime_variety = 0;
    float color_variety = 0;
    float velocity_variety = 0;

};
#pragma pack(pop)

struct Particle
{
    Vector2 position{0,0};
    Vector2 direction{0,0};
    float velocity = 0;
    float size = 0;
    float velocity_slowdown = 0;
    Color color = BLANK;
    float lifetime = 0;
};

#define RESPAWN_PARTICLE_EFFECT ParticleEffect{20, {0,0}, {0,0}, 1.5f, 200.0f, 100.0f, 6.0f, {12, 232, 78, 255}, 360.0f, 25.0f, 2.0f, 25.0f, 1.25f, 0.25f, 50.0f};

class Particles
{
    public:
    GameClient* game;
    std::vector<Particle> HandledParticles;
    Particles();
    ~Particles();
    Particles(GameClient* game);

    void Clear();
    void Update();
    void Quit();

    void CreateParticles(ParticleEffect effect);
    void CreateParticles(
        int amount = 0,
        Vector2 impact = {0, 0},
        Vector2 direction = {0, 0},
        float lifetime = 0,
        float velocity = 0,
        float velocity_slowdown = 0,
        float size = 0,
        Color color = BLANK,
        float angle_variety = 0,
        float impact_variety = 0,
        float size_variety = 0,
        float velocity_slowdown_variety = 0,
        float lifetime_variety = 0,
        float color_variety = 0,
        float velocity_variety = 0);
};


#endif //BURST_PARTICLES_H