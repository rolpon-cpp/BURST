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

    PlayerToUpdate->CurrentState.id = PlayerToUpdate->PlayerID;
    PlayerToUpdate->LastState.id = PlayerToUpdate->PlayerID;
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

void PlayerDamageAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    PlayerDamage damage;
    memcpy(&damage, &Packet.data, sizeof(PlayerDamage));
    if (OurServer.Players.contains(damage.id))
    {
        ENetPacket* GetChunkENetPacket = enet_packet_create(&Packet, sizeof(struct Packet), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(OurServer.Players[damage.id], 0, GetChunkENetPacket);
    }
}
