//
// Created by lalit on 5/22/2026.
//

#ifndef BURST_PARTICLES_H
#define BURST_PARTICLES_H

#include "../game_libs.h"

class GameClient;

struct ParticleEffect
{

};

class Particles
{
    public:
    GameClient* game;
    Particles();
    ~Particles();
    Particles(GameClient* game);
    void Init(GameClient* game);
    void Update();
    void Quit();
};


#endif //BURST_PARTICLES_H