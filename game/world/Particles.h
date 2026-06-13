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
    Vector2 impact = {0, 0};
    Vector2 direction = {0, 0};
    int amount = 0;
    float lifetime = 0;
    Color color = BLANK;

    float directionVariety = 0;
    float impactVariety = 0;
    float lifetimeVariety = 0;
    float colorVariety = 0;
};
#pragma pack(pop)

struct Particle
{
    Vector2 position;
    Vector2 direction;
    float velocity;
    Color color;
    float lifetime;
};

class Particles
{
    public:
    GameClient* game;
    std::vector<Particle> particles;
    Particles();
    ~Particles();
    Particles(GameClient* game);
    void CreateParticles(ParticleEffect effect);
    void Update();
    void Quit();
};


#endif //BURST_PARTICLES_H