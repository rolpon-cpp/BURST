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
struct PlayerState {
    int32_t id = 0;
    Vector2 position = {0,0};
    Vector2 velocity = {0,0};
    float speed = 0;
    double timestamp = 0;
};
#pragma pack(pop)

bool CompareStates(PlayerState State1, PlayerState State2);

class Game;

class Player {
public:
    Game *game;
    int32_t PlayerID = -1;
    PlayerState CurrentState = { 0 };
    PlayerState LastState = { 0 };
    PlayerState LocalState = { 0 };
    std::vector<PlayerState> PreviousPlayerStates;
    Player(float X, float Y, float Speed, Game* game);
    Player(PlayerState State, Game* game);
    Player();
    ~Player();
    void Update();
    void SmoothPlayerState(double ServerTime, double Delay, bool Extrapolate = true);
    void MovePlayer(double ServerTime);
};


#endif //ALLS_PLAYER_H