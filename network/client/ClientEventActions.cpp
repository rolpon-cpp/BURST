//
// Created by lalit on 5/10/2026.
//

#include "ClientEventActions.h"

#include <algorithm>
#include <iostream>

#include "../Utils.h"
#include "../../game/Map.h"
#include "../../game/Game.h"
#include "Client.h"
#include "../Packet.h"

void TimeSyncAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    double CurrentTime = GetTimeUtils();
    OurClient.ServerTimeOffset = Packet.timestamp + static_cast<double>(Event.peer->
        roundTripTime) / 2000.0f - CurrentTime;
    OurClient.Ping = Event.peer->roundTripTime / 1000.0f;
}

void PlayerJoinAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    PlayerJoin player_join;
    memcpy(&player_join, &Packet.data, sizeof(PlayerJoin));
    PlayerState s = {
        player_join.id,
        player_join.starting_location.x,
        player_join.starting_location.y,
        {0,0},
        {0,0},
        0,
        100,
        0,
        Packet.timestamp
    };
    OurClient.OtherPlayers[player_join.id] = Player(s, OurClient.game);
    OurClient.OtherPlayers[player_join.id].PlayerID = player_join.id;
}

void PlayerLeftAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    PlayerLeft player_left;
    memcpy(&player_left, &Packet.data, sizeof(PlayerLeft));
    if (OurClient.OtherPlayers.contains(player_left.id))
        OurClient.OtherPlayers.erase(player_left.id);
}

void PlayerUpdateAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    double ServerTime = OurClient.GetServerTime();

    PlayerState NewState;
    memcpy(&NewState, &Packet.data, sizeof(PlayerState));
    PlayerState OldState;
    if (OurClient.OtherPlayers.contains(NewState.id))
    {
        OldState = OurClient.OtherPlayers.at(NewState.id).CurrentState;

        OurClient.OtherPlayers[NewState.id].CurrentState = NewState;
        OurClient.OtherPlayers[NewState.id].LastState = OldState;

        OurClient.OtherPlayers[NewState.id].PreviousPlayerStates.push_back(NewState);
        OurClient.OtherPlayers[NewState.id].PreviousPlayerStates.erase(
            std::remove_if(
                OurClient.OtherPlayers[NewState.id].PreviousPlayerStates.begin(),
                OurClient.OtherPlayers[NewState.id].PreviousPlayerStates.end(),
                [ServerTime](PlayerState& p) { return ServerTime - p.timestamp >= 4.0f; }
            ),
            OurClient.OtherPlayers[NewState.id].PreviousPlayerStates.end()
        );
    }
}

void GetChunkAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    ChunkUpload ServerChunkUpload{};

    memcpy(&ServerChunkUpload, &Packet.data, sizeof(ChunkUpload));

    OurClient.game->MainMap.SetChunk(&ServerChunkUpload.Chunk, ServerChunkUpload.ChunkPos.x, ServerChunkUpload.ChunkPos.y);
}

void HealthUpdateAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    PlayerHealthUpdate health_update;
    memcpy(&health_update, &Packet.data, sizeof(health_update));

    OurClient.game->MainPlayer.CurrentState.health = health_update.health;
    OurClient.game->MainPlayer.LocalState.health = health_update.health;
}

void PlayerCharacterResetAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    PlayerCharacterReset resetChar;
    memcpy(&resetChar, &Packet.data, sizeof(PlayerCharacterReset));

    OurClient.OurPlayerID = resetChar.id;
    OurClient.game->MainPlayer.CurrentState.id = resetChar.id;
    OurClient.game->MainPlayer.PlayerID = resetChar.id;

    OurClient.game->MainPlayer.CurrentState.position = resetChar.position;
    OurClient.game->MainPlayer.CurrentState.health = resetChar.health;
    OurClient.game->MainPlayer.CurrentState.speed = resetChar.speed;
}
