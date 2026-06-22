//
// Created by  on 5/10/2026.
//

#ifndef BURST_EVENTACTIONS_H
#define BURST_EVENTACTIONS_H

#include "../../game_libs.h"

class Client;
struct Packet;

void TimeSyncAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void PlayerJoinAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void PlayerLeftAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void PlayerUpdateAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void GetChunkAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void ServerPropertiesUpdateAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void PlayerCharacterResetAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void AnimationEventAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void FeedbackEventAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void BulletSpawnEventAction(Client& OurClient, Packet& Packet, ENetEvent& Event);
void BulletDespawnEventAction(Client& OurClient, Packet& Packet, ENetEvent& Event);

#endif //BURST_EVENTACTIONS_H