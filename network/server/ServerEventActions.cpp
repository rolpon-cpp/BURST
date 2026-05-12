//
// Created by lalit on 5/11/2026.
//

#include "ServerEventActions.h"
#include "../Player.h"
#include "../Packet.h"

void PlayerUpdateAction(Server& OurServer, Packet& Packet, ENetEvent& Event)
{
    auto* PlayerToUpdate = static_cast<Player*>(Event.peer->data);
    PlayerToUpdate->LastState = PlayerToUpdate->CurrentState;

    memcpy(&PlayerToUpdate->CurrentState, &Packet.data, sizeof(PlayerState));

    PlayerToUpdate->CurrentState.id = PlayerToUpdate->PlayerID;
    PlayerToUpdate->LastState.id = PlayerToUpdate->PlayerID;
}
