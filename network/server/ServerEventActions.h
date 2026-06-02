//
// Created by lalit on 5/11/2026.
//

#ifndef BURST_SERVEREVENTACTIONS_H
#define BURST_SERVEREVENTACTIONS_H

#include "../../game_libs.h"

class Server;
struct Packet;

void PlayerUpdateAction(Server& OurServer, Packet& Packet, ENetEvent& Event);
void PlayerDashAction(Server& OurServer, Packet& Packet, ENetEvent& Event);
void PlayerWeaponAttackAction(Server& OurServer, Packet& Packet, ENetEvent& Event);
void GetChunkAction(Server& OurServer, Packet& Packet, ENetEvent& Event);

#endif //BURST_SERVEREVENTACTIONS_H