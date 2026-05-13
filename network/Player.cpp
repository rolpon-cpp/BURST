//
// Created by lalit on 11/27/2025.
//

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include "raylib.h"

#include "Player.h"

#include <iostream>
#include <ostream>

#include "raymath.h"
#include "../game/Game.h"
#include "client/Client.h"

// Returns true if the states are significantly different
bool CompareStates(PlayerState State1, PlayerState State2)
{
    if (Vector2Distance(State1.position, State2.position) > 0)
        return true;
    if (Vector2Distance(State1.velocity, State2.velocity) > 0)
        return true;
    if (abs(State1.speed - State2.speed) > 0)
        return true;
    return false;
}

Player::Player()
{
}

Player::Player(float X, float Y, float Speed, Game* game)
{
    this->game = game;
    PlayerID = -1;
    CurrentState.position = {X, Y};
    CurrentState.speed = Speed;
    CurrentState.timestamp = 0;
    LastState = CurrentState;
    LocalState = CurrentState;
}

Player::Player(PlayerState State, Game* game)
{
    this->game = game;
    PlayerID = -1;
    CurrentState = State;
    LastState = CurrentState;
    LocalState = CurrentState;
}

Player::~Player()
{
}

void Player::SmoothPlayerState(double ServerTime, double Delay, bool Extrapolate)
{
    this->LocalState.id = this->CurrentState.id;

    double render_time = ServerTime - Delay;
    PlayerState l = { 0 };
    l.timestamp = -1;
    PlayerState h = { 0 };
    h.timestamp = FLT_MAX;
    PlayerState& lowest = l;
    PlayerState& highest = h;

    bool found = false;
    bool found2 = false;

    for (PlayerState &s : this->PreviousPlayerStates) {
        if (s.timestamp < render_time && s.timestamp > lowest.timestamp) {
            lowest = s;
            found = true;
        }
    }

    for (PlayerState &s : this->PreviousPlayerStates) {
        if (s.timestamp > render_time && s.timestamp < highest.timestamp && s.position != lowest.position) {
            highest = s;
            found2 = true;
        }
    }

    if (!found2)
    {
        if (Extrapolate)
        {
            highest.position.x = lowest.position.x + lowest.velocity.x * lowest.speed * (render_time - lowest.timestamp);
            highest.position.y = lowest.position.y + lowest.velocity.y * lowest.speed * (render_time - lowest.timestamp);
        } else
        {
            highest = CurrentState;
        }
    }

    if (found)
    {
        double time_diff = highest.timestamp - lowest.timestamp;
        float prog = (render_time - lowest.timestamp) / time_diff;

        this->LocalState.position = {lowest.position.x + (highest.position.x - lowest.position.x) * prog, lowest.position.y + (highest.position.y - lowest.position.y) * prog};
    }

}

void Player::MovePlayer(double ServerTime)
{
    LastState = CurrentState;
    Vector2 MyPlayerVelocity = {0, 0};
    if (IsKeyDown(KEY_A))
        MyPlayerVelocity.x -= 1;
    if (IsKeyDown(KEY_D))
        MyPlayerVelocity.x += 1;
    if (IsKeyDown(KEY_W))
        MyPlayerVelocity.y -= 1;
    if (IsKeyDown(KEY_S))
        MyPlayerVelocity.y += 1;
    MyPlayerVelocity = Vector2Normalize(MyPlayerVelocity);

    CurrentState.velocity = MyPlayerVelocity;

    MyPlayerVelocity.x *= CurrentState.speed;
    MyPlayerVelocity.y *= CurrentState.speed;

    CurrentState.position.x += MyPlayerVelocity.x * GetFrameTime();
    CurrentState.position.y += MyPlayerVelocity.y * GetFrameTime();

    CurrentState.timestamp = ServerTime;
    LocalState = CurrentState;
}

void Player::Update()
{
    string tex = "player2";
    if (PlayerID < 0)
        tex = "player1";
    DrawTextureEx(game->GameResources.Textures[tex], LocalState.position - Vector2{18.0f, 18.0f}, 0, 0.5f, WHITE);
}
