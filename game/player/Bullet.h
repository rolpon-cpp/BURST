#ifndef BURST_BULLET_H
#define BURST_BULLET_H
#include "../../game_libs.h"
#include <cstdint>

#pragma pack(push, 1)
struct BulletData
{
    uint64_t id = 0;
    int32_t owner_id = -1;
    Vector2 position = {0, 0};
    Vector2 direction = {0, 0};
    uint8_t color[3] = {0, 0, 0};
    float speed = 0;
    float size = 0;
    float height = 0;
    float damage = 0;
    double timestamp = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct BulletDespawn
{
    uint64_t id = -1;
};
#pragma pack(pop)

class Game;

class Bullet
{
    public:
        bool MarkedForDeletion;
        Game* game;
        Vector2 CurrentPosition;
        BulletData MyBulletData;
        Bullet();
        Bullet(Game* game, BulletData data);
        ~Bullet();
        void Update();
};


#endif //BURST_BULLET_H