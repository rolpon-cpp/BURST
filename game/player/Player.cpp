//
// Created by  on 11/27/2025.
//

#include "../../game_libs.h"

#include "Player.h"

#include <cfloat>
#include <cstdint>
#include <iostream>
#include <ostream>

#include "raymath.h"
#include "../core/Game.h"
#include "../../network/Utils.h"
#include "../../network/client/Client.h"

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
    if (abs(State1.rotation - State2.rotation) > 0)
        return true;
    if (abs(State1.health - State2.health) > 0)
        return true;
    return false;
}

bool DetectIllegalStates(PlayerState BaseState, PlayerState SuspectedState)
{
    float Delta = SuspectedState.timestamp - BaseState.timestamp;

    Vector2 ExpectedPosition = {
        BaseState.position.x + (BaseState.velocity.x * Delta) + (BaseState.direction.x * BaseState.speed * Delta),
        BaseState.position.y + (BaseState.velocity.y * Delta) + (BaseState.direction.y * BaseState.speed * Delta)
    };

    if (Vector2Distance(SuspectedState.position, ExpectedPosition) >= 300)
        return true;

    if (Vector2Distance({0, 0}, SuspectedState.direction) > 1.0f)
        return true;

    return false;
}

Player::Player(float X, float Y, float Speed, Game* game)
{
    //printf("creating player ?? (properties)\n");
    ////std::cout << std::flush;
    this->game = game;
    PlayerID = -1;
    CurrentState.position = {X, Y};
    CurrentState.health = 100.0f;
    CurrentState.speed = Speed;
    CurrentState.timestamp = 0;
    DisplayHealth = 100.0f;
    LastZoned = 0.0f;
    LastDashed = 0;
    LastState = CurrentState;
    LocalState = CurrentState;

    inventory = Inventory(game, this);
    if (!game->IsClient)
    {
        inventory.GiveItem(make_shared<ProjectileWeapon>(&inventory,WeaponData{
            "pistol", PROJECTILE, 20.0f, 0.5f, 999.0f, 0.1f, 0.0f, 1, 100
        }));
    }
}

Player::Player(PlayerState State, Game* game)
{
    //printf("creating player (state)%i\n", State.id);
    //std::cout << std::flush;
    this->game = game;
    PlayerID = State.id;
    CurrentState = State;
    LastState = CurrentState;
    LocalState = CurrentState;
    DisplayHealth = CurrentState.health;
    LastDashed = 0;
    LastZoned = 0.0f;

    inventory = Inventory(game, this);
    if (!game->IsClient)
    {
        inventory.GiveItem(make_shared<ProjectileWeapon>(&inventory,WeaponData{
            "pistol", PROJECTILE, 20.0f, 0.5f, 999.0f, 0.1f, 0.0f, 1, 100
        }));
    }
}

Player::Player()
{
}

Player::~Player()
{

}

bool Player::IsLocalPlayer()
{
    if (game == nullptr)
        return false;
    if (!game->IsClient)
        return false;
    return PlayerID < 0 || PlayerID == ((GameClient*)game)->MainClient.OurPlayerID;
}

PlayerState Player::GetPlayerState(double Timestamp)
{
    PlayerState l = { 0 };
    l.timestamp = -1;
    PlayerState h = { 0 };
    h.timestamp = FLT_MAX;

    PlayerState& lowest = l;
    PlayerState& highest = h;

    bool FoundBefore = false;
    bool FoundAfter = false;

    for (PlayerState &s : this->PreviousPlayerStates) {
        if (s.timestamp < Timestamp && s.timestamp > lowest.timestamp) {
            lowest = s;
            FoundBefore = true;
        }
    }

    for (PlayerState &s : this->PreviousPlayerStates) {
        if (s.timestamp > Timestamp && s.timestamp < highest.timestamp && s.position != lowest.position) {
            highest = s;
            FoundAfter = true;
        }
    }

    if (FoundBefore && FoundAfter)
    {
        PlayerState SmoothedState = CurrentState;

        double time_diff = highest.timestamp - lowest.timestamp;
        float prog;
        if (time_diff == 0)
            prog = 1.0f;
        else
            prog = (Timestamp - lowest.timestamp) / time_diff;

        SmoothedState.rotation = lowest.rotation + (highest.rotation - lowest.rotation) * prog;
        SmoothedState.position = {lowest.position.x + (highest.position.x - lowest.position.x) * prog, lowest.position.y + (highest.position.y - lowest.position.y) * prog};

        return SmoothedState;
    }
    if (FoundBefore)
    {
        PlayerState StateGuess = lowest;
        double Delta = Timestamp - lowest.timestamp;
        if (Delta <= 0)
            return lowest;
        if (Delta >= 0.1f)
            Delta = 0.1f;

        ProcessVelocity(&StateGuess, Delta);
        ProcessDirection(&StateGuess, Delta);

        StateGuess.position = Vector2Lerp(StateGuess.position, CurrentState.position, 0.5f);

        return StateGuess;
    }
    if (!FoundBefore && FoundAfter)
        return highest;

    return CurrentState;
}

void Player::SmoothPlayerState(double Delay)
{
    PlayerState PredictedState = GetPlayerState(((GameClient*)game)->MainClient.GetServerTime() - Delay);
    Vector2 PrevPos = LocalState.position;
    LocalState = PredictedState;
    LocalState.position = Vector2Lerp(PrevPos, LocalState.position, game->GetDeltaTime() * 24.0f);
}

Vector2 Player::ProcessInputs()
{
    if (ZoneTarget == -1)
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
        if (IsKeyPressed(KEY_SPACE) && Vector2Distance({0, 0}, CurrentState.velocity) < 500 && ((GameClient*)game)->MainClient.GetServerTime() - LastDashed >= 1)
        {
            CurrentState.velocity = Vector2Normalize(
                ((GameClient*)game)->MainCamera.GetWorldMousePos() - GetCenter()) * 2500.0f;
            LastDashed = ((GameClient*)game)->MainClient.GetServerTime();
            IsDashing = true;
            DashedPlayerID = -1;
        }
        if (IsKeyPressed(KEY_F) && Vector2Distance({0, 0}, CurrentState.velocity) < 500 && game->GetTime() - LastZoned >= 1.0f)
        {
            int32_t id = -1;
            for (auto &[plr_id, plr] : ((GameClient*)game)->MainClient.Players)
            {
                if (Vector2Distance(plr.GetCenter(), ((GameClient*)game)->MainCamera.GetWorldMousePos()) <= 100.0f &&
                    game->MainMap.CastRay(CurrentState.position, plr.CurrentState.position).hitTile == nullptr)
                {
                    id = plr_id;
                    break;
                }
            }

            if (id != -1)
            {
                ZoneTarget = id;
                LastZoned = game->GetTime();
            }
        }
        MyPlayerDirection = Vector2Normalize(MyPlayerDirection);
        CurrentState.rotation = 180.0f - Vector2LineAngle(GetCenter(), ((GameClient*)game)->MainCamera.GetWorldMousePos()) * RAD2DEG;
        return MyPlayerDirection;
    }
    if (((GameClient*)game)->MainClient.Players.contains(ZoneTarget))
    {
        Player& plr = ((GameClient*)game)->MainClient.Players[ZoneTarget];
        CurrentState.rotation = 180.0f - Vector2LineAngle(GetCenter(), plr.GetCenter()) * RAD2DEG;
    }
    return {0,0};
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

void Player::ProcessMovementAttacks(PlayerState* State)
{
    float VelocityMagnitude = Vector2Distance({0, 0}, State->velocity);

    if (VelocityMagnitude < 100 && IsDashing)
    {
        IsDashing = false;
        DashedPlayerID = -1;
    }

    if (IsDashing)
    {
        for (auto &[id, player] : ((GameClient*)game)->MainClient.Players)
        {
            if (id == PlayerID)
                continue;
            if (id == DashedPlayerID)
                continue;
            if (CheckCollisionRecs({State->position.x, State->position.y, 36, 36},
                                   {player.CurrentState.position.x - 4.5f, player.CurrentState.position.y - 4.5f, 45, 45}))
            {
                SoundEffect sound_effect =
                {
                    "dash_hit",
                    1.0f,
                    1.0f,
                };
                sound_effect.set_properties(0);
                ((GameClient*)game)->MainSounds.PlayGameSound(sound_effect);

                ((GameClient*)game)->MainClient.MovementAttack(State->position, min(max(VelocityMagnitude / 150.0f, 0.0f), 20.0f));
                DashedPlayerID = player.CurrentState.id;
                ((GameClient*)game)->MainCamera.ShakeCamera(1.0f);
                break;
            }
        }
    } else if (ZoneTarget != -1 && ((GameClient*)game)->MainClient.Players.contains(ZoneTarget))
    {
        State->velocity = Vector2Normalize(((GameClient*)game)->MainClient.Players[ZoneTarget].CurrentState.position-State->position) * 2000.0f;
        float TargetDist = Vector2Distance(State->position, ((GameClient*)game)->MainClient.Players[ZoneTarget].CurrentState.position);
        float Percent = (TargetDist / 600.0f);
        if (Percent >= 1.0f)
            Percent = 1.0f;
        if (Percent <= 0.0f)
            Percent = 0.0f;

        ((GameClient*)game)->MainCamera.ZoomCamera(max(3.0f - Percent*3.0f, 1.0f));
        if (TargetDist <= 50.0f)
        {
            SoundEffect sound_effect =
            {
                "dash_hit",
                1.0f,
                1.0f,
            };
            sound_effect.set_properties(0);
            ((GameClient*)game)->MainSounds.PlayGameSound(sound_effect);

            ((GameClient*)game)->MainClient.MovementAttack(State->position, min(max(VelocityMagnitude / 400.0f, 0.0f), 20.0f));
            ((GameClient*)game)->MainCamera.ShakeCamera(0.2f);
            ZoneTarget = -1;
        }
        if (ZoneTarget != -1 && game->GetTime() - LastZoned >= 1.0f)
        {
            ZoneTarget = -1;
        }
    }
}

Vector2 Player::GetCenter()
{
    return Vector2{LocalState.position.x + 18.0f, LocalState.position.y + 18.0f};
}

void Player::ProcessDirection(PlayerState* State, float Delta, int Steps)
{
    Vector2 Direction = State->direction;
    Direction.x *= State->speed * Delta;
    Direction.y *= State->speed * Delta;

    Direction.x += State->velocity.x * Delta;
    Direction.y += State->velocity.y * Delta;

    for (int i = 0; i < Steps; i++)
    {
        float DeltaX = (Direction.x / Steps) * (i + 1);
        float DeltaY = (Direction.y / Steps) * (i + 1);

        Rectangle xCheck = {State->position.x + DeltaX, State->position.y, 36.0f, 36.0f};
        if (game->MainMap.CollisionCheck(xCheck))
            DeltaX = 0.0f;
        State->position.x += DeltaX;

        Rectangle yCheck = {State->position.x, State->position.y + DeltaY, 36.0f, 36.0f};
        if (game->MainMap.CollisionCheck(yCheck))
            DeltaY = 0.0f;
        State->position.y += DeltaY;
    }
}

void Player::MovePlayer(Vector2 Direction, float Delta, bool UseLocalState)
{
    PlayerState* FinalState = UseLocalState ? &LocalState : &CurrentState;
    LastState = *FinalState;
    FinalState->direction = Direction;

    ProcessVelocity(FinalState,Delta);
    ProcessDirection(FinalState, Delta);
    if (IsLocalPlayer())
        ProcessMovementAttacks(FinalState);

    FinalState->timestamp = ((GameClient*)game)->MainClient.GetServerTime();
    LocalState = *FinalState;
    if (IsLocalPlayer())
        ((GameClient*)game)->MainClient.UpdateState(CurrentState);
}

void Player::Update()
{
    inventory.Owner = this;
    inventory.game = game;
    //printf("processing player! %i\n",PlayerID);
    //cout << this << "\n" << std::flush;

    //printf("inventory process 1\n");
    inventory.Update();

    //printf("health limit process 2\n");
    CurrentState.health = max(min(CurrentState.health, 100.0f), 0.0f);
    LocalState.health = max(min(LocalState.health, 100.0f), 0.0f);

    //printf("player movement process 3\n");
    if (IsLocalPlayer()) // checks if we're the local player
        MovePlayer(CurrentState.health > 0 ? ProcessInputs() : Vector2{0, 0}, game->GetDeltaTime());

    if (game->IsClient)
    {
        //cout << inventory.EquippedItemIdx << endl;
        if (IsMouseButtonPressed(0) && IsLocalPlayer() && CurrentState.health > 0)
        {
            inventory.Attack(((GameClient*)game)->MainCamera.GetWorldMousePos());
        }
        //printf("rendering process 4\n");
        string playerName = "Player " + to_string(PlayerID);
        if (IsLocalPlayer())
            playerName = "You";
        int sz = MeasureText(playerName.c_str(), 20);

        DisplayHealth = Lerp(DisplayHealth, CurrentState.health, 5.0f * game->GetDeltaTime());
        float healthSZ =max(min((float)DisplayHealth, 100.0f), 0.0f);
        DrawRectangleRounded({LocalState.position.x - 32, LocalState.position.y - 17.5f, 100, 12.5f}, 0.35f, 2, RED);
        DrawRectangleRounded({LocalState.position.x - 32 + (100 - healthSZ), LocalState.position.y - 17.5f, healthSZ, 12.5f}, 0.5f, 2, GREEN);

        DrawText(playerName.c_str(),LocalState.position.x + 18 - sz/2,LocalState.position.y - 37.5f, 20, BLACK);
        DrawTexturePro(((GameClient*)game)->MainResources.GetTexture(tex), {0, 0, 72.0f, 72.0f}, {LocalState.position.x + 18.0f, LocalState.position.y + 18.0f, 36.0f, 36.0f}, {18.0f,18.0f}, LocalState.rotation, WHITE);
    }

    //std::cout << "DONE PROCESSING PLAYER!" << "\n" << std::flush;
}

void Player::Destroy()
{
    //printf("destroying player %i\n", PlayerID);
    inventory.Destroy();
}
