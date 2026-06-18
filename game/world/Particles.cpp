//
// Created by  on 5/22/2026.
//

#include "Particles.h"

#include <iostream>
#include <ostream>

#include "raymath.h"
#include "../core/Game.h"

Particles::Particles()
{
}

Particles::~Particles()
{
}

Particles::Particles(GameClient* game)
{
    this->game = game;
}

void Particles::CreateParticles(int amount, Vector2 impact, Vector2 direction, float lifetime, float velocity,
    float velocity_slowdown, float size, Color color, float direction_variety, float impact_variety, float size_variety,
    float velocity_slowdown_variety, float lifetime_variety, float color_variety, float velocity_variety)
{
    CreateParticles({amount, impact, direction, lifetime, velocity, velocity_slowdown, size, color, direction_variety, impact_variety, size_variety,
    velocity_slowdown_variety, lifetime_variety, color_variety, velocity_variety});
}

void Particles::CreateParticles(ParticleEffect effect)
{
    for (int i = 0; i < effect.amount; i++)
    {
        Particle newParticle{};

        Vector2 randomVariation = Vector2{GetRandomValue(-effect.impact_variety * 10.0f, effect.impact_variety * 10.0f) / 10.0f,
        GetRandomValue(-effect.impact_variety * 10.0f, effect.impact_variety * 10.0f) / 10.0f};

        float dirAngle = 180.0f - Vector2LineAngle({0,0}, effect.direction) * RAD2DEG;
        dirAngle -= GetRandomValue(-effect.angle_variety * 10.0f, effect.angle_variety * 10.0f) / 10.0f;
        effect.direction = Vector2Normalize(Vector2Rotate({0,0}, dirAngle));

        newParticle.position = Vector2Add(randomVariation, effect.impact);
        newParticle.color = ColorBrightness(effect.color, GetRandomValue(-effect.color_variety * 100.0f, effect.color_variety * 100.0f) / 100.0f);
        newParticle.lifetime = effect.lifetime + GetRandomValue(-effect.lifetime_variety * 10.0f, effect.lifetime_variety * 10.0f) / 10.0f;
        newParticle.velocity = effect.velocity + GetRandomValue(-effect.velocity_variety * 10.0f, effect.velocity_variety * 10.0f) / 10.0f;
        newParticle.size = effect.size + GetRandomValue(-effect.size_variety * 10.0f, effect.size_variety * 10.0f) / 10.0f;
        newParticle.velocity_slowdown = effect.velocity_slowdown + GetRandomValue(-effect.velocity_slowdown_variety * 10.0f, effect.velocity_slowdown_variety * 10.0f) / 10.0f;

        HandledParticles.push_back(newParticle);
    }
}

void Particles::Clear()
{
    HandledParticles.clear();
}

void Particles::Update()
{
    std::erase_if(HandledParticles, [this](Particle& p)
    {
       if (p.lifetime <= 0.0f)
           return true;
        if (HandledParticles.size() > 200)
            return true;

        return false;
    });

    for (int i = 0; i < HandledParticles.size(); i++)
    {
        Particle& p = HandledParticles[i];

        if (p.velocity > 0)
        {
            p.position.x += p.direction.x * p.velocity * game->GetDeltaTime();
            p.position.y += p.direction.y * p.velocity * game->GetDeltaTime();
            p.velocity -= p.velocity_slowdown * game->GetDeltaTime();
        } else
        {
            p.velocity = 0.0f;
        }

        float natural_transparency = 1.0f;
        if (p.lifetime <= 0.1f)
            natural_transparency = p.lifetime / 0.1f;

        DrawRectanglePro({p.position.x, p.position.y, p.size, p.size}, {p.size / 2.0f, p.size / 2.0f}, p.lifetime * RAD2DEG,
            ColorAlpha(p.color, natural_transparency));
        DrawCircleGradient(p.position.x, p.position.y, p.size * 1.1f,
            ColorAlpha(p.color, natural_transparency / 6.0f),
            ColorAlpha(p.color, natural_transparency / 12.0f));

        p.lifetime -= game->GetDeltaTime();
    }
}

void Particles::Quit()
{
    Clear();
}
