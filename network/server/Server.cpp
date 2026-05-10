//
// Created by 7906200 on 11/14/2025.
//

#include "Server.h"
#include "../Player.h"
#include <iostream>
#include <raymath.h>
#include <thread>
#include <unordered_map>
#include "../Packet.h"
#include "../Utils.h"
#include "enet/enet.h"

using namespace std;

ENetHost* server = {0};
bool ServerRunning = true;
unordered_map<int32_t, ENetPeer*> peers;

double LastSyncedTime = GetTimeUtils();

void StartServer(std::string IPAddress, int Port, int MaxClients)
{
    ServerRunning = true;

    ENetAddress server_address = {0};
    ENetEvent server_event;

    enet_address_set_host_ip(&server_address, IPAddress.c_str());
    server_address.port = Port;

    server = enet_host_create(&server_address, MaxClients, 2, 0, 0);

    if (server == NULL)
    {
        std::cout << "Error creating server\n";
    }

    std::cout << "server is running\n";

    int32_t latest_player_id = 1;

    while (ServerRunning)
    {
        int active = enet_host_service(server, &server_event, 50);
        if (active > 0)
        {
            switch (server_event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                {
                    printf("A new client connected from %x:%u.\n", server_event.peer->address.host,
                           server_event.peer->address.port);
                    Player* newPlayer = new Player();
                    newPlayer->PlayerID = latest_player_id;
                    server_event.peer->data = newPlayer;
                    peers[latest_player_id] = server_event.peer;

                    // Sending time sync to new player
                    Packet myPacket = {};
                    myPacket.type = TIME_SYNC;
                    myPacket.timestamp = GetTimeUtils();
                    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(server_event.peer, 0, packet);

                    for (auto [name,peer] : peers)
                    {
                        auto* p = static_cast<Player*>(peer->data);
                        if (p->PlayerID != newPlayer->PlayerID)
                        {
                            // Sending player join to existing player
                            myPacket = {};
                            myPacket.type = PLAYER_JOIN;
                            myPacket.timestamp = GetTimeUtils();

                            PlayerJoin playerJoin = {0};
                            playerJoin.id = newPlayer->PlayerID;
                            playerJoin.starting_location = {0, 0};
                            memcpy(&myPacket.data, &playerJoin, sizeof(playerJoin));

                            packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
                            enet_peer_send(peer, 0, packet);

                            // Sending player join to new player
                            myPacket = {};
                            myPacket.timestamp = GetTimeUtils();
                            myPacket.type = PLAYER_JOIN;

                            playerJoin = {0};
                            playerJoin.id = p->PlayerID;
                            playerJoin.starting_location = p->CurrentState.position;
                            memcpy(&myPacket.data, &playerJoin, sizeof(playerJoin));

                            packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
                            enet_peer_send(server_event.peer, 0, packet);

                        }
                    }
                    latest_player_id += 1;
                    std::cout << "added " << latest_player_id << "\n";
                    break;
                }
            case ENET_EVENT_TYPE_RECEIVE:
                {
                    Packet plrPacket;
                    if (server_event.packet->dataLength != sizeof(Packet))
                    {
                        enet_packet_destroy(server_event.packet);
                        break;
                    }
                    memcpy(&plrPacket, server_event.packet->data, server_event.packet->dataLength);
                    switch (plrPacket.type)
                    {
                    case PLAYER_UPDATE:
                        {
                            auto* player = static_cast<Player*>(server_event.peer->data);
                            player->LastState = player->CurrentState;

                            memcpy(&player->CurrentState, &plrPacket.data, sizeof(PlayerState));

                            player->CurrentState.id = player->PlayerID;
                            player->LastState.id = player->PlayerID;
                            break;
                        }
                    }
                    enet_packet_destroy(server_event.packet);

                    break;
                }
            case ENET_EVENT_TYPE_DISCONNECT:
                {
                    printf("disconnected.\n");
                    Player* oldPlayer = reinterpret_cast<Player*>(server_event.peer->data);
                    for (auto [name, peer] : peers)
                    {
                        auto* p = static_cast<Player*>(peer->data);
                        if (p->PlayerID != oldPlayer->PlayerID)
                        {
                            Packet myPacket;
                            myPacket.type = PLAYER_LEFT;
                            myPacket.timestamp = GetTimeUtils();

                            PlayerLeft left = {0};
                            left.id = oldPlayer->PlayerID;
                            memcpy(&myPacket.data, &left, sizeof(left));

                            ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket),
                                                                    ENET_PACKET_FLAG_RELIABLE);
                            enet_peer_send(peer, 0, packet);
                        }
                    }
                    peers.erase(oldPlayer->PlayerID);
                    delete oldPlayer;
                    cout << "new size " << peers.size() << "\n";
                    break;
                }
            case ENET_EVENT_TYPE_NONE:
                break;
            }
        }
        if (GetTimeUtils() - LastSyncedTime >= 1)
        {
            for (auto [id, peer] : peers)
            {
                Packet myPacket;
                myPacket.type = TIME_SYNC;
                myPacket.timestamp = GetTimeUtils();
                ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(peer, 0, packet);
            }
            LastSyncedTime = GetTimeUtils();
        }
        if (peers.size() > 1)
        {
            for (auto [id, peer] : peers)
            {
                for (auto [other_id, other_peer] : peers)
                {
                    if (other_id != id)
                    {
                        auto* player = reinterpret_cast<Player*>(other_peer->data);
                        Packet myPacket;
                        myPacket.type = PLAYER_UPDATE;
                        myPacket.timestamp = player->CurrentState.timestamp;

                        memcpy(&myPacket.data, &player->CurrentState, sizeof(player->CurrentState));

                        ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
                        enet_peer_send(peer, 0, packet);
                    }
                }
            }
        }

        enet_host_flush(server);
    }
}

void StopServer()
{
    if (server != nullptr)
        enet_host_destroy(server);
}
