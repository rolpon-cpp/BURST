//
// Created by lalit on 5/11/2026.
//

#include "ServerEventActions.h"

#include <iostream>

#include "Server.h"
#include "../../game/Player.h"
#include "../../game/Map.h"
#include "../../game/Game.h"
#include "../Packet.h"
#include "../Utils.h"

void PlayerUpdateAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    auto* PlayerToUpdate = static_cast<Player*>(Event.peer->data);
    PlayerToUpdate->LastState = PlayerToUpdate->CurrentState;

    memcpy(&PlayerToUpdate->CurrentState, &Packet.data, sizeof(PlayerState));
    
    // Ownership modifications
    PlayerToUpdate->CurrentState.health = PlayerToUpdate->LastState.health;
    PlayerToUpdate->CurrentState.id = PlayerToUpdate->PlayerID;
    PlayerToUpdate->LastState.id = PlayerToUpdate->PlayerID;

    // Timestamp anticheat check
    if (abs(PlayerToUpdate->CurrentState.timestamp - GetTimeUtils()) >= 1.0f)
        PlayerToUpdate->CurrentState.timestamp = GetTimeUtils();

    // Health anticheat check
    if (PlayerToUpdate->CurrentState.health <= 0.0f)
        PlayerToUpdate->CurrentState.position = PlayerToUpdate->LastState.position;

    // Teleportation anticheat check
    if (Vector2Distance(PlayerToUpdate->CurrentState.position, PlayerToUpdate->LastState.position) >= 500.0f)
    {
        PlayerToUpdate->CurrentState.position = PlayerToUpdate->LastState.position;

        struct Packet myPacket = {};
        myPacket.type = PLAYER_CHAR_RESET;
        myPacket.timestamp = GetTimeUtils();

        PlayerCharacterReset charReset = {0};
        charReset.id = PlayerToUpdate->PlayerID;
        charReset.position = PlayerToUpdate->LastState.position;
        charReset.health = PlayerToUpdate->CurrentState.health;
        charReset.timestamp = GetTimeUtils();
        charReset.speed = PlayerToUpdate->CurrentState.speed;

        memcpy(&myPacket.data, &charReset, sizeof(charReset));

        ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(Event.peer, 0, packet);
    }

    // Adding player states to list
    PlayerToUpdate->PreviousPlayerStates.push_back(PlayerToUpdate->CurrentState);
    PlayerToUpdate->PreviousPlayerStates.erase(
            std::remove_if(
                PlayerToUpdate->PreviousPlayerStates.begin(),
                PlayerToUpdate->PreviousPlayerStates.end(),
                [](PlayerState& p) { return GetTimeUtils() - p.timestamp >= 4.0f; }
            ),
            PlayerToUpdate->PreviousPlayerStates.end()
        );
}

void GetChunkAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    ChunkRequest PlayerChunkRequest{};
    memcpy(&PlayerChunkRequest, &Packet.data, sizeof(ChunkRequest));

    Chunk* c = OurServer.game->MainMap.GetChunk(PlayerChunkRequest.ChunkPos.x,PlayerChunkRequest.ChunkPos.y);
    if (c == nullptr)
        return;

    struct Packet PacketData;
    PacketData.type = GET_CHUNK;
    PacketData.timestamp = GetTimeUtils();
    memset(&PacketData.data, 0, sizeof(PacketData.data));

    ChunkUpload PlayerChunkUpload{};
    PlayerChunkUpload.ChunkPos = PlayerChunkRequest.ChunkPos;
    PlayerChunkUpload.Chunk = *c;

    memcpy(&PacketData.data, &PlayerChunkUpload, sizeof(ChunkUpload));

    ENetPacket* GetChunkENetPacket = enet_packet_create(&PacketData, sizeof(struct Packet), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(Event.peer, 0, GetChunkENetPacket);
}

void PlayerDashAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    Player* DashingPlayer = (Player*)Event.peer->data;
    if (GetTimeUtils() - DashingPlayer->LastDashed < 1.0f)
        return;

    DashingPlayer->LastDashed = GetTimeUtils();

    PlayerDash dash;
    memcpy(&dash, &Packet.data, sizeof(PlayerDash));

    PlayerState DashingPlayerState = DashingPlayer->GetPlayerState(Packet.timestamp);

    // Distance Anticheat check
    if (Vector2Distance(DashingPlayerState.position, dash.impact) >= 50)
        dash.impact = DashingPlayerState.position;

    // Health Anticheat check
    if (DashingPlayer->CurrentState.health <= 0.0f)
        return;

    // Damage Anticheat limit
    dash.damage = min(max(dash.damage, 0.0f), 20.0f);

    for (auto &[id, peer] : OurServer.Players)
    {
        if (id == DashingPlayer->PlayerID)
            continue;
        Player* VictimPlayer = (Player*) peer->data;
        PlayerState VictimPlayerState = VictimPlayer->GetPlayerState(Packet.timestamp);

        if (CheckCollisionRecs({dash.impact.x, dash.impact.y, 36, 36}, {VictimPlayerState.position.x, VictimPlayerState.position.y, 36, 36}))
            VictimPlayer->CurrentState.health -= dash.damage;
    }
}
