//
// Created by 7906200 on 11/14/2025.
//

#ifndef SERVER_H
#define SERVER_H

#include "../../game_libs.h"
#include <string>
#include <unordered_map>
#include "../Packet.h"
#include "../../game/Player.h"

using namespace std;
class Game;

class Server
{
    long LatestPlayerID;
    unordered_map<PacketType, void(*)(Server& OurServer, Packet& Packet, ENetEvent& Event)> PacketEventActions;

    double LastSyncedTime;

    void HandleEvents();
    void HandleTimeSync();
    void HandlePlayerStates();

    void PlayerConnect(ENetEvent& Event);
    void PlayerDisconnect(ENetEvent& Event);

    void PlayerTimeSync(ENetPeer* Peer);
    void PlayerCreateCharacter(ENetPeer* Peer);
    void PlayerUpdateNotification(Player* UpdatedPlayer, ENetPeer* PeerToNotify);
    void PlayerJoinNotification(ENetPeer* NewPeer, ENetPeer* PeerToNotify);
    void PlayerLeftNotification(ENetPeer* OldPeer, ENetPeer* PeerToNotify);

public:
    Game* game;
    unordered_map<int32_t, ENetPeer*> Players;
    ENetHost* Host = nullptr;
    bool Running;
    Server();
    Server(Game* game);
    ~Server();
    void Reset();
    void StartServer(std::string IPAddress = "127.0.0.1", int Port = 5000, int MaxClients = 32);
    void UpdateServer();
    void StopServer();
};

#endif //SERVER_H
