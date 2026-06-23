//
// Created by  on 11/27/2025.
//

#ifndef ALLS_PLAYER_H
#define ALLS_PLAYER_H
#include <cstdint>
#include <vector>
#include "raymath.h"
#include "Weapon.h"

enum FeedbackType
{
    WEAPON, MOVEMENT
};

#pragma pack(push, 1)
struct PlayerScoreFeedback {
    float pts = 0;
    FeedbackType type;
    Vector2 impact{0, 0};
    uint8_t clr[3] {0};
    double timestamp = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerCustomizedItems {
    int32_t id = -1;
    uint8_t name[32] = {0};
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerJoin {
    int32_t id = 0;
    Vector2 starting_location = {0, 0};
    PlayerCustomizedItems customized_items;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerLeft {
    int32_t id = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerMovementAttack {
    Vector2 impact = {0,0};
    float damage = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerServerProperties {
    float health = 0;
    WeaponData idx1;
    WeaponData idx2;
    WeaponData idx3;
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
    WeaponState weapon_state = WeaponState{};
    double timestamp = 0;
    Vector2 GetCenter();
};
#pragma pack(pop)

bool CompareStates(PlayerState State1, PlayerState State2);
bool DetectIllegalStates(PlayerState BaseState, PlayerState SuspectedState);

class Game;
class GameClient;

class Player {

public:
    Game *game;
    int32_t PlayerID = -1;

    PlayerCustomizedItems CustomizedItems;

    Inventory inventory;

    PlayerState CurrentState = { 0 };
    PlayerState LastState = { 0 };
    PlayerState LocalState = { 0 };
    std::vector<PlayerState> PreviousPlayerStates;

    int32_t DashedPlayerID = -1;
    float DashCharge = 0.0f;

    bool IsDashing = false;
    int32_t ZoneTarget = -1;
    double LastMovementAttack = 0.0f;

    Vector2 LastGhostPos{0,0};
    std::vector<std::pair<PlayerState,double>> Ghosts;

    double DisplayHealth = 0;

    float Points;
    std::vector<PlayerScoreFeedback> Feedback;

    Player();
    Player(float X, float Y, float Speed, Game* game);
    Player(PlayerState State, Game* game);
    ~Player();

    void Update();
    Vector2 GetCenter();

    void ProcessVelocity(PlayerState* State, float Delta);
    void ProcessMovementAttacks(PlayerState* State);
    void ProcessDirection(PlayerState* State, float Delta, int Steps = 1);

    void SmoothPlayerState(double Delay);
    PlayerState GetPlayerState(double Timestamp);

    Vector2 ProcessInputs();
    void MovePlayer(Vector2 Direction, float Delta, bool UseLocalState = false);
    bool IsLocalPlayer();

    void Destroy();
};


#endif //ALLS_PLAYER_H