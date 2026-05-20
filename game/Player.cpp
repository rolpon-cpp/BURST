//
// Created by lalit on 11/27/2025.
//

#include "../game_libs.h"

#include "Player.h"

#include <iostream>
#include <ostream>

#include "raymath.h"
#include "Game.h"
#include "../network/Utils.h"
#include "../network/client/Client.h"

// Returns true if the states are different
bool CompareStates(PlayerState State1, PlayerState State2)
{
    if (Vector2Distance(State1.position, State2.position) > 0)
        return true;
    if (Vector2Distance(State1.direction, State2.direction) > 0)
        return true;
    if (Vector2Distance(State1.velocity, State2.velocity) > 0)
        return true;
    if (abs(State1.speed - State2.speed) > 0)
        return true;
    if (abs(State1.health - State2.health) > 0)
        return true;
    return false;
}

Player::Player()
{
}

Player::Player(float X, float Y, float Speed, GameClient* game)
{
    this->game = game;
    PlayerID = -1;
    CurrentState.position = {X, Y};
    CurrentState.health = 100.0f;
    CurrentState.speed = Speed;
    CurrentState.timestamp = 0;
    DisplayHealth = 100.0f;
    LastDashed = 0;
    LastState = CurrentState;
    LocalState = CurrentState;
}

Player::Player(PlayerState State, GameClient* game)
{
    this->game = game;
    PlayerID = -1;
    CurrentState = State;
    LastState = CurrentState;
    LocalState = CurrentState;
    DisplayHealth = CurrentState.health;
    LastDashed = 0;
}

Player::~Player()
{
}

void Player::SmoothPlayerState(double Delay, bool Extrapolate)
{
    this->LocalState.id = this->CurrentState.id;

    double render_time = game->MainClient.GetServerTime() - Delay;
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
            float extrapolated_dt = (render_time - lowest.timestamp);
            MovePlayer(CurrentState.direction, extrapolated_dt, true);
            highest = LocalState;
        } else
        {
            PlayerState f = CurrentState;
            highest = f;
            highest.timestamp = render_time;
        }
    }

    Vector2 p = LocalState.position;
    LocalState = CurrentState;
    LocalState.position = p;

    if (found)
    {
        double time_diff = highest.timestamp - lowest.timestamp;
        float prog;
        if (time_diff == 0)
            prog = 1.0f;
        else
            prog = (render_time - lowest.timestamp) / time_diff;

        this->LocalState.position = {lowest.position.x + (highest.position.x - lowest.position.x) * prog, lowest.position.y + (highest.position.y - lowest.position.y) * prog};
    }

}

Vector2 Player::ProcessInputs()
{
    Vector2 MyPlayerDirection = {0, 0};
    if (IsKeyDown(KEY_A))
        MyPlayerDirection.x -= 1;
    if (IsKeyDown(KEY_D))
        MyPlayerDirection.x += 1;
    if (IsKeyDown(KEY_W))
        MyPlayerDirection.y -= 1;
    if (IsKeyDown(KEY_S))
        MyPlayerDirection.y += 1;
    if (IsKeyPressed(KEY_SPACE) && GetTimeUtils() - LastDashed >= 1)
    {
        CurrentState.velocity = Vector2Normalize(
            GetScreenToWorld2D(GetMousePosition(), game->MainCamera.RaylibCamera) - (CurrentState.position + Vector2{
                18, 18
            })) * 2500.0f;
        LastDashed = GetTimeUtils();
    }
    MyPlayerDirection = Vector2Normalize(MyPlayerDirection);
    return MyPlayerDirection;
}

void Player::ProcessVelocity(PlayerState* State, float Delta)
{
    bool S1x = State->velocity.x > 0;
    bool S1y = State->velocity.y > 0;
    State->velocity -= Vector2Normalize(State->velocity) * 6000.0f * Delta;
    bool S2x = State->velocity.x > 0;
    bool S2y = State->velocity.y > 0;
    if (S1x != S2x)
        State->velocity.x = 0;
    if (S1y != S2y)
        State->velocity.y = 0;
}

void Player::ProcessDashing(PlayerState* State)
{
    if (Vector2Distance({0, 0}, State->velocity) <= 100)
        DashedPlayerIDs.clear();
    else
    {
        for (auto& [id, player] : game->MainClient.OtherPlayers)
        {
            if (PlayerID == id)
                continue;
            bool HasDashedBefore = false;
            for (int32_t o_id : DashedPlayerIDs)
            {
                if (o_id == id)
                {
                    HasDashedBefore = true;
                    break;
                }
            }
            if (HasDashedBefore)
                continue;

            if (CheckCollisionRecs({State->position.x, State->position.y, 36, 36},
                                   {player.LocalState.position.x, player.LocalState.position.y, 36, 36}))
            {
                game->MainClient.DamagePlayer(id, 20.0f);
                DashedPlayerIDs.push_back(id);
            }
        }
    }
}

void Player::ProcessDirection(PlayerState* State, float Delta)
{
    Vector2 Direction = State->direction;
    Direction.x *= State->speed * Delta;
    Direction.y *= State->speed * Delta;

    Direction.x += State->velocity.x * Delta;
    Direction.y += State->velocity.y * Delta;

    Rectangle xCheck = {State->position.x + Direction.x, State->position.y, 36.0f, 36.0f};
    if (game->MainMap.CollisionCheck(xCheck))
        Direction.x = 0.0f;
    State->position.x += Direction.x;

    Rectangle yCheck = {State->position.x, State->position.y + Direction.y, 36.0f, 36.0f};
    if (game->MainMap.CollisionCheck(yCheck))
        Direction.y = 0.0f;
    State->position.y += Direction.y;
}

void Player::MovePlayer(Vector2 Direction, float Delta, bool UseLocalState)
{
    PlayerState* FinalState = UseLocalState ? &LocalState : &CurrentState;
    LastState = *FinalState;
    FinalState->direction = Direction;

    ProcessVelocity(FinalState,Delta);
    ProcessDirection(FinalState, Delta);
    ProcessDashing(FinalState);

    FinalState->timestamp = game->MainClient.GetServerTime();
    LocalState = *FinalState;
    if (PlayerID < 0)
        game->MainClient.UpdateState(CurrentState);
}

void Player::Update()
{
    if (PlayerID < 0) // checks if we're the local player & we are alive
        MovePlayer(CurrentState.health > 0 ? ProcessInputs() : Vector2{0, 0}, GetFrameTime());

    string tex = "player2";
    if (PlayerID < 0)
        tex = "player1";
    string playerName = "Player " + to_string(PlayerID);
    if (PlayerID < 0)
        playerName = "You";
    int sz = MeasureText(playerName.c_str(), 20);

    DisplayHealth = Lerp(DisplayHealth, CurrentState.health, 5.0f * GetFrameTime());
    float healthSZ =max(min(DisplayHealth, 100.0f), 0.0f);
    DrawRectangleRounded({LocalState.position.x - 32, LocalState.position.y - 17.5f, 100, 12.5f}, 0.35f, 2, RED);
    DrawRectangleRounded({LocalState.position.x - 32 + (100 - healthSZ), LocalState.position.y - 17.5f, healthSZ, 12.5f}, 0.5f, 2, GREEN);

    DrawText(playerName.c_str(),LocalState.position.x + 18 - sz/2,LocalState.position.y - 37.5f, 20, BLACK);
    DrawTextureEx(game->MainResources.Textures[tex], LocalState.position, 0, 0.5f, WHITE);
}
