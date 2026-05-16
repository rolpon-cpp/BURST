//
// Created by lalit on 5/10/2026.
//

#ifndef BURST_EVENTACTIONS_H
#define BURST_EVENTACTIONS_H

#include "enet/enet.h"

class Client;
struct Packet;

void TimeSyncAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void PlayerJoinAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void PlayerLeftAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void PlayerUpdateAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void GetChunkAction(Client& OurClient, Packet& Packet, ENetEvent& Event);

#endif //BURST_EVENTACTIONS_H