//
// Created by lalit on 11/16/2025.
//

#ifndef ALLS_CLIENT_H
#define ALLS_CLIENT_H

#include "../../game_libs.h"
#include <string>
#include <unordered_map>
#include "../../game/Player.h"
#include "../Packet.h"

using namespace std;

class Game;
class GameClient;

class Client
{
public:

    GameClient* game;

    bool Connected;

    ENetHost* Host = nullptr;
    ENetPeer* Peer = nullptr;
    unordered_map<int32_t, Player> OtherPlayers;
    unordered_map<PacketType, void(*)(Client& OurClient, Packet& Packet, ENetEvent& Event)> EventActions;

    double ServerTimeOffset;
    double LastUpdatedState;
    double Ping;

    int32_t OurPlayerID;

    Client(GameClient* game);
    Client();
    ~Client();
    void Connect(std::string IPAddress, int Port);
    void Disconnect();
    void Update();
    void Reset();

    std::unordered_map<int32_t, Player>& GetPlayers();
    void UpdateState(PlayerState& State);
    void RequestChunk(Vector2 Position);
    void DashIntoPlayer(int32_t ID, Vector2 ImpactPoint, float Damage);
    double GetServerTime();
};

#endif //ALLS_CLIENT_H