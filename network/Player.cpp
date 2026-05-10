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

#include "client/Client.h"

Player::Player()
{
}

Player::Player(float X, float Y, float Speed)
{
    CurrentState.position = {X, Y};
    CurrentState.speed = Speed;
    CurrentState.timestamp = 0;
    LastState = CurrentState;
    LocalState = CurrentState;
}

Player::Player(PlayerState State)
{
    CurrentState = State;
    LastState = CurrentState;
    LocalState = CurrentState;
}

Player::~Player()
{
}

void Player::SmoothPlayerState(double ServerTime, double Delay)
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
        highest.position.x = lowest.position.x + lowest.velocity.x * lowest.speed * (render_time - lowest.timestamp);
        highest.position.y = lowest.position.y + lowest.velocity.y * lowest.speed * (render_time - lowest.timestamp);
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
