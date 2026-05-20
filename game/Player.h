//
// Created by lalit on 11/27/2025.
//

#ifndef ALLS_PLAYER_H
#define ALLS_PLAYER_H
#include <vector>
#include "raymath.h"

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
struct PlayerDamage {
    int32_t id = 0;
    float damage = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerState {
    int32_t id = 0;
    Vector2 position = {0,0};
    Vector2 direction = {0,0};
    Vector2 velocity = {0, 0};
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
    GameClient *game;
    int32_t PlayerID = -1;
    PlayerState CurrentState = { 0 };
    PlayerState LastState = { 0 };
    PlayerState LocalState = { 0 };
    std::vector<PlayerState> PreviousPlayerStates;
    std::vector<int32_t> DashedPlayerIDs;
    double DisplayHealth;
    double LastDashed;

    Player(float X, float Y, float Speed, GameClient* game);
    Player(PlayerState State, GameClient* game);
    Player();
    ~Player();
    void Update();
    void SmoothPlayerState(double Delay, bool Extrapolate = true);
    void ProcessVelocity(PlayerState* State, float Delta);
    void ProcessDashing(PlayerState* State);
    void ProcessDirection(PlayerState* State, float Delta);
    Vector2 ProcessInputs();
    void MovePlayer(Vector2 Direction, float Delta, bool UseLocalState = false);
};


#endif //ALLS_PLAYER_H