//
// Created by lalit on 5/10/2026.
//

#include "ClientEventActions.h"

#include <algorithm>
#include <iostream>

#include "../Utils.h"
#include "../../game/world/WorldMap.h"
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
        WeaponState{},
        Packet.timestamp
    };
    OurClient.Players[player_join.id] = Player(s, OurClient.game);
    OurClient.Players[player_join.id].PlayerID = player_join.id;
}

void PlayerLeftAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    PlayerLeft player_left;
    memcpy(&player_left, &Packet.data, sizeof(PlayerLeft));
    if (OurClient.Players.contains(player_left.id))
        OurClient.Players.erase(player_left.id);
}

void PlayerUpdateAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    double ServerTime = OurClient.GetServerTime();

    PlayerState NewState;
    memcpy(&NewState, &Packet.data, sizeof(PlayerState));
    PlayerState OldState;
    if (OurClient.Players.contains(NewState.id))
    {
        OldState = OurClient.Players.at(NewState.id).CurrentState;

        OurClient.Players[NewState.id].CurrentState = NewState;
        OurClient.Players[NewState.id].LastState = OldState;

        OurClient.Players[NewState.id].PreviousPlayerStates.push_back(NewState);
        OurClient.Players[NewState.id].PreviousPlayerStates.erase(
            std::remove_if(
                OurClient.Players[NewState.id].PreviousPlayerStates.begin(),
                OurClient.Players[NewState.id].PreviousPlayerStates.end(),
                [ServerTime](PlayerState& p) { return ServerTime - p.timestamp >= 4.0f; }
            ),
            OurClient.Players[NewState.id].PreviousPlayerStates.end()
        );
    }

}

void GetChunkAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    ChunkUpload ServerChunkUpload{};

    memcpy(&ServerChunkUpload, &Packet.data, sizeof(ChunkUpload));

    OurClient.game->MainMap.SetChunk(&ServerChunkUpload.Chunk, ServerChunkUpload.ChunkPos.x, ServerChunkUpload.ChunkPos.y);
}

void ServerPropertiesUpdateAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    PlayerServerProperties serv_properties;
    memcpy(&serv_properties, &Packet.data, sizeof(serv_properties));

    OurClient.game->MainPlayer.inventory.SetItem(serv_properties.idx1, 0);
    OurClient.game->MainPlayer.inventory.SetItem(serv_properties.idx2, 1);
    OurClient.game->MainPlayer.inventory.SetItem(serv_properties.idx3, 2);

    OurClient.game->MainPlayer.CurrentState.health = serv_properties.health;
    OurClient.game->MainPlayer.LocalState.health = serv_properties.health;
}

void PlayerCharacterResetAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    PlayerState resetChar;
    memcpy(&resetChar, &Packet.data, sizeof(resetChar));

    OurClient.game->MainPlayer.CurrentState = resetChar;
    OurClient.game->MainPlayer.LastState = resetChar;
    OurClient.game->MainPlayer.LocalState = resetChar;
}

void AnimationEventAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    AnimationEvent event;
    memcpy(&event, &Packet.data, sizeof(event));

    OurClient.game->MainAnimator.Animate(event);
}
