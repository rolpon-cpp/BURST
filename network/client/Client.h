//
// Created by lalit on 11/16/2025.
//

#ifndef ALLS_CLIENT_H
#define ALLS_CLIENT_H
#include <string>
#include <unordered_map>
#include "enet/enet.h"
#include "../Player.h"
#include "../Packet.h"

using namespace std;

class Game;

class Client
{
public:

    Game* game;

    bool Connected;

    ENetHost* Host = nullptr;
    ENetPeer* Peer = nullptr;
    unordered_map<int32_t, Player> OtherPlayers;
    unordered_map<PacketType, void(*)(Client& OurClient, Packet& Packet, ENetEvent& Event)> EventActions;

    double ServerTimeOffset;
    double LastUpdatedState;

    Client(Game* game);
    Client();
    ~Client();
    void Connect(std::string IPAddress, int Port);
    void Disconnect();
    void Update();
    void Reset();

    std::unordered_map<int32_t, Player>& GetPlayers();
    void UpdateState(PlayerState& State);
    double GetServerTime();
};

#endif //ALLS_CLIENT_H