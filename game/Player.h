//
// Created by lalit on 11/27/2025.
//

#ifndef ALLS_PLAYER_H
#define ALLS_PLAYER_H
#include <cstdint>
#include <vector>
#include "raymath.h"

#define PLR_DASH_COOLDOWN 1

#pragma pack(push, 1)
struct PlayerJoin {
    int32_t id = 0;
    Vector2 starting_location = {0, 0};
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerLeft {
    int32_t id = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerDash {
    Vector2 impact = {0,0};
    float damage = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerHealthUpdate {
    float health = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerState {
    int32_t id = 0;
    Vector2 position = {0,0};
    Vector2 direction = {0,0};
    Vector2 velocity = {0, 0};
    float rotation = 0;
    float health = 0;
    float speed = 0;
    double timestamp = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerCharacterReset {
    int32_t id = 0;
    Vector2 position = {0,0};
    float health = 0;
    float speed = 0;
    double timestamp = 0;
};
#pragma pack(pop)

bool CompareStates(PlayerState State1, PlayerState State2);

class Game;
class GameClient;

class Player {

public:
    Game *game;
    int32_t PlayerID = -1;
    PlayerState CurrentState = { 0 };
    PlayerState LastState = { 0 };
    PlayerState LocalState = { 0 };
    std::vector<PlayerState> PreviousPlayerStates;

    int32_t DashedPlayerID = -1;
    double LastDashed = 0.0f;
    bool IsDashing = false;

    double DisplayHealth;

    Player(float X, float Y, float Speed, Game* game);
    Player(PlayerState State, Game* game);
    Player();
    ~Player();

    void Update();
    Vector2 GetCenter();

    void ProcessVelocity(PlayerState* State, float Delta);
    void ProcessDashing(PlayerState* State);
    void ProcessDirection(PlayerState* State, float Delta, int Steps = 1);

    void SmoothPlayerState(double Delay);
    PlayerState GetPlayerState(double Timestamp);

    Vector2 ProcessInputs();
    void MovePlayer(Vector2 Direction, float Delta, bool UseLocalState = false);
    bool IsLocalPlayer();
};


#endif //ALLS_PLAYER_H