#include "Bullet.h"

#include <iostream>
#include <ostream>

#include "../core/Game.h"
#include "raymath.h"

Bullet::Bullet()
{
}

Bullet::Bullet(Game* game, BulletData data)
{
    this->MyBulletData = data;
    this->game = game;
    this->CurrentPosition = data.position;
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
    if (game->GetServerTime() - MyBulletData.timestamp >= 10.0f)
    {
        MarkedForDeletion = true;
        return;
    }

    if (MarkedForDeletion)
        return;

    Vector2 RealDirection = Vector2Normalize(MyBulletData.direction);
    CurrentPosition = MyBulletData.position + (RealDirection * MyBulletData.speed * (game->GetServerTime() - MyBulletData.timestamp));

    if (game->MainMap.CollisionCheck({CurrentPosition.x - MyBulletData.size,
            CurrentPosition.y - MyBulletData.size,
            MyBulletData.size * 2.0f, MyBulletData.size * 2.0f}))
    {
        MarkedForDeletion = true;
        return;
    }

    if (!game->IsClient)
    {
        auto* game_server = dynamic_cast<GameServer*>(game);
        for (auto &[id, peer] : game_server->MainServer.Players)
        {
            Player* player = (Player*)peer->data;
            if (player->PlayerID != MyBulletData.owner_id && CheckCollisionCircleRec(CurrentPosition, MyBulletData.size, {player->CurrentState.position.x,
                player->CurrentState.position.y, 36.0f, 36.0f}))
            {
                player->CurrentState.health -= MyBulletData.damage;
                MarkedForDeletion = true;

                if (game_server->MainServer.Players.contains(MyBulletData.owner_id))
                {
                    Player* owner = (Player*)game_server->MainServer.Players[MyBulletData.owner_id]->data;
                    PlayerScoreFeedback feedback{};

                    feedback.impact = CurrentPosition;
                    feedback.pts = MyBulletData.damage * 1.5f;
                    feedback.timestamp = game_server->GetServerTime();
                    feedback.type = WEAPON;
                    feedback.clr[0] = GREEN.r;
                    feedback.clr[1] = GREEN.g;
                    feedback.clr[2] = GREEN.b;

                    owner->Feedback.push_back(feedback);
                    owner->Points += feedback.pts;
                    game_server->MainServer.SendPacket(game_server->MainServer.Players[owner->PlayerID],
                        PLAYER_ATTACK_FEEDBACK, &feedback,sizeof(PlayerScoreFeedback));
                }

                return;
            }
        }
    } else
    {
        auto* game_client = dynamic_cast<GameClient*>(game);
        for (auto &[id, player] : game_client->MainClient.Players)
        {
            if (player.PlayerID != MyBulletData.owner_id && CheckCollisionCircleRec(CurrentPosition, MyBulletData.size, {player.CurrentState.position.x,
                player.CurrentState.position.y, 36.0f, 36.0f}))
            {
                MarkedForDeletion = true;
                return;
            }
        }
        DrawCircleGradient(CurrentPosition.x, CurrentPosition.y, MyBulletData.size * 2.0f, ORANGE, BLANK);
        DrawCircleV(CurrentPosition, MyBulletData.size, RED);
    }

}
