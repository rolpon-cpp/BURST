//
// Created by lalit on 5/10/2026.
//

#include "EventActions.h"
#include "../Utils.h"
#include "Client.h"
#include "../Packet.h"

void TimeSyncAction(Client& OurClient, Packet& Packet, ENetEvent& Event)
{
    double CurrentTime = GetTimeUtils();
    OurClient.ServerTimeOffset = Packet.timestamp + static_cast<double>(Event.peer->
        roundTripTime) / 2000.0f - CurrentTime;
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
        0,
        Packet.timestamp
    };
    OurClient.OtherPlayers[player_join.id] = Player(s);
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
                [ServerTime](PlayerState& p) { return ServerTime - p.timestamp >= 1.0f; }
            ),
            OurClient.OtherPlayers[NewState.id].PreviousPlayerStates.end()
        );
    }
}
