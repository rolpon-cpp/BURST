//
// Created by 7906200 on 11/14/2025.
//

#ifndef SERVER_H
#define SERVER_H
#include <string>
#include <unordered_map>
#include "../Packet.h"
#include "enet/enet.h"

using namespace std;

class Server
{
public:
    ENetHost* Host = nullptr;
    bool Running;
    long LatestPlayerID;

    unordered_map<int32_t, ENetPeer*> Players;
    unordered_map<PacketType, void(*)(Server& OurServer, Packet& Packet, ENetEvent& Event)> PacketEventActions;

    double LastSyncedTime;

    void HandleEvents();
    void HandleTimeSync();
    void HandlePlayerStates();

    void PlayerConnect(ENetEvent& Event);
    void PlayerDisconnect(ENetEvent& Event);

    Server();
    ~Server();
    void Reset();
    void StartServer(std::string IPAddress = "127.0.0.1", int Port = 5000, int MaxClients = 32);
    void UpdateServer();
    void StopServer();
};

#endif //SERVER_H
