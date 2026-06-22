//
// Created by  on 5/11/2026.
//

#include "ServerEventActions.h"

#include <iostream>

#include "Server.h"
#include "../../game/player/Player.h"
#include "../../game/world/WorldMap.h"
#include "../../game/core/Game.h"
#include "../Packet.h"
#include "../Utils.h"

void PlayerUpdateAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    auto* PlayerToUpdate = static_cast<Player*>(Event.peer->data);

    PlayerState PreviousPlayerState = PlayerToUpdate->CurrentState;

    memcpy(&PlayerToUpdate->CurrentState, &Packet.data, sizeof(PlayerState));
    
    // Ownership modifications
    PlayerToUpdate->CurrentState.health = PreviousPlayerState.health;
    PlayerToUpdate->CurrentState.id = PlayerToUpdate->PlayerID;
    PreviousPlayerState.id = PlayerToUpdate->PlayerID;
    PlayerToUpdate->LastState.id = PlayerToUpdate->PlayerID;

    // Weapon equip
    PlayerToUpdate->inventory.EquipItem(PlayerToUpdate->CurrentState.weapon_state.inventoryIdx);

    // Timestamp anticheat check
    if (abs(PlayerToUpdate->CurrentState.timestamp - OurServer.game->GetLocalTime()) >= 1.0f)
        PlayerToUpdate->CurrentState.timestamp = OurServer.game->GetLocalTime();

    // Health anticheat check
    if (PlayerToUpdate->CurrentState.health <= 0.0f)
        PlayerToUpdate->CurrentState.position = PreviousPlayerState.position;

    // Illegal state anticheat check
    if (DetectIllegalStates(PreviousPlayerState, PlayerToUpdate->CurrentState))
    {
        PlayerToUpdate->CurrentState = PreviousPlayerState;

        struct Packet myPacket = {};
        myPacket.type = PLAYER_CHAR_RESET;
        myPacket.timestamp = OurServer.game->GetLocalTime();

        memcpy(&myPacket.data, &PreviousPlayerState, sizeof(PreviousPlayerState));

        ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(Event.peer, 0, packet);
    }

    // Adding player states to list
    PlayerToUpdate->PreviousPlayerStates.push_back(PlayerToUpdate->CurrentState);
    PlayerToUpdate->PreviousPlayerStates.erase(
            std::remove_if(
                PlayerToUpdate->PreviousPlayerStates.begin(),
                PlayerToUpdate->PreviousPlayerStates.end(),
                [&OurServer](PlayerState& p) { return OurServer.game->GetLocalTime() - p.timestamp >= 4.0f; }
            ),
            PlayerToUpdate->PreviousPlayerStates.end()
        );

    PlayerToUpdate->LastState = PlayerToUpdate->CurrentState;
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
    PacketData.timestamp = OurServer.game->GetLocalTime();
    memset(&PacketData.data, 0, sizeof(PacketData.data));

    ChunkUpload PlayerChunkUpload{};
    PlayerChunkUpload.ChunkPos = PlayerChunkRequest.ChunkPos;
    PlayerChunkUpload.Chunk = *c;

    memcpy(&PacketData.data, &PlayerChunkUpload, sizeof(ChunkUpload));

    ENetPacket* GetChunkENetPacket = enet_packet_create(&PacketData, sizeof(struct Packet), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(Event.peer, 0, GetChunkENetPacket);
}

void PlayerMovementAttackAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    Player* AttackingPlayer = (Player*)Event.peer->data;
    if (OurServer.game->GetLocalTime() - AttackingPlayer->LastMovementAttack < 0.95f)
        return;

    AttackingPlayer->LastMovementAttack = OurServer.game->GetLocalTime();

    PlayerMovementAttack atk;
    memcpy(&atk, &Packet.data, sizeof(PlayerMovementAttack));

    PlayerState AttackingPlayerState = AttackingPlayer->GetPlayerState(Packet.timestamp);

    // Distance Anticheat check
    if (Vector2Distance(AttackingPlayerState.GetCenter(), atk.impact) >= 50)
        atk.impact = AttackingPlayerState.GetCenter();

    // Health Anticheat check
    if (AttackingPlayer->CurrentState.health <= 0.0f)
        return;

    // Damage Anticheat limit
    atk.damage = min(max(atk.damage, 0.0f), 50.0f);

    for (auto &[id, peer] : OurServer.Players)
    {
        if (id == AttackingPlayer->PlayerID)
            continue;
        Player* VictimPlayer = (Player*) peer->data;
        if (VictimPlayer->CurrentState.health <= 0.0f)
            continue;
        PlayerState VictimPlayerState = VictimPlayer->GetPlayerState(Packet.timestamp);

        if (Vector2Distance(VictimPlayerState.GetCenter(), atk.impact)<=50)
        {
            // damage person
            VictimPlayer->CurrentState.health -= atk.damage;

            // damage feedback
            PlayerScoreFeedback feedback{};

            feedback.impact = atk.impact;
            feedback.pts = atk.damage * 2.5f;
            feedback.timestamp = OurServer.game->GetLocalTime();
            feedback.type = MOVEMENT;
            feedback.clr[0] = ORANGE.r;
            feedback.clr[1] = ORANGE.g;
            feedback.clr[2] = ORANGE.b;

            OurServer.SendPacket(Event.peer,
                PLAYER_ATTACK_FEEDBACK, &feedback, sizeof(PlayerScoreFeedback));

            // play sound to all
            AnimationEvent event;
            event.type = SoundAnimationEvent;
            event.use_position = false;
            event.player_id = AttackingPlayer->PlayerID;
            event.position = AttackingPlayerState.position;
            event.sound_effect =
            {
                "dash_hit",
                1.0f,
                1.0f,
            };
            OurServer.SendPacketToAll(ANIMATION, &event, sizeof(event), {AttackingPlayer->PlayerID});

            // end loop
            break;
        }
    }
}

void PlayerWeaponAttackAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    Player* AttackingPlayer = (Player*)Event.peer->data;
    WeaponAttack attackInfo;
    memcpy(&attackInfo, &Packet.data, sizeof(WeaponAttack));

    if (AttackingPlayer->CurrentState.health <= 0.0f)
        return;

    if (Vector2Distance(attackInfo.origin, AttackingPlayer->CurrentState.position) >= 100.0f)
        attackInfo.origin = AttackingPlayer->CurrentState.position;

    if (abs(GetTimeUtils() - attackInfo.timestamp) >= 1.0f)
        attackInfo.timestamp = GetTimeUtils();

    attackInfo.inventoryIdx = max(min(attackInfo.inventoryIdx, 2), 0);

    AttackingPlayer->inventory.Attack(attackInfo);
}

void PlayerRespawnRequestAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    Player* RespawningPlayer = (Player*)Event.peer->data;
    if (RespawningPlayer->CurrentState.health <= 0.0f)
    {
        RespawningPlayer->CurrentState.health = 100.0f;
        RespawningPlayer->CurrentState.position = OurServer.GetSpawnLocation();
        RespawningPlayer->LastState = RespawningPlayer->CurrentState;
        RespawningPlayer->LocalState = RespawningPlayer->CurrentState;

        struct Packet myPacket = {};
        myPacket.type = PLAYER_CHAR_RESET;
        myPacket.timestamp = OurServer.game->GetLocalTime();

        memcpy(&myPacket.data, &RespawningPlayer->CurrentState, sizeof(RespawningPlayer->CurrentState));

        AnimationEvent respawnAnimationEvent = {ParticleAnimationEvent, true, RespawningPlayer->CurrentState.position, -1};
        respawnAnimationEvent.particle_effect = RESPAWN_PARTICLE_EFFECT;
        OurServer.SendPacketToAll(ANIMATION, &respawnAnimationEvent, sizeof(AnimationEvent));

        OurServer.SendPacket(Event.peer, myPacket);
    }
}

void PlayerReloadRequestAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    if (((Player*)Event.peer)->CurrentState.health <= 0)
        return;
    ((Player*)Event.peer)->inventory.Reload();
}
