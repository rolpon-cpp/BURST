//
// Created by lalit on 11/27/2025.
//

#ifndef ALLS_PLAYER_H
#define ALLS_PLAYER_H
#include <raymath.h>
#include <vector>

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

class Player {
public:
    int32_t PlayerID = -1;
    PlayerState CurrentState = { 0 };
    PlayerState LastState = { 0 };
    PlayerState LocalState = { 0 };
    std::vector<PlayerState> PreviousPlayerStates;
    Player(float X, float Y, float Speed);
    Player(PlayerState State);
    Player();
    ~Player();
    void SmoothPlayerState(double ServerTime, double Delay);
    void MovePlayer(double ServerTime);
};


#endif //ALLS_PLAYER_H