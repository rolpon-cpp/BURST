//
// Created by 7906200 on 11/14/2025.
//

#include "Server.h"
#include <iostream>
#include <ranges>

#include "../Utils.h"
#include "../../game/Player.h"
#include "ServerEventActions.h"
#include "../../game/Game.h"

using namespace std;

Server::Server()
{
    Reset();
    game = nullptr;
}

Server::Server(Game* game)
{
    this->game = game;
    Reset();
}

Server::~Server()
{
}

void Server::Reset()
{
    if (Host != nullptr)
        delete Host;
    Players.clear();
    Host = nullptr;
    Running = false;
    LastSyncedTime = 0.0f;
    LatestPlayerID = -1;
    PacketEventActions.clear();
    PacketEventActions[PLAYER_UPDATE] = &PlayerUpdateAction;
    PacketEventActions[GET_CHUNK] = &GetChunkAction;
}

void Server::StartServer(std::string IPAddress, int Port, int MaxClients)
{
    if (Running)
        return;
    Reset();
    printf("Starting server...\n");

    ENetAddress Address;

    enet_address_set_host_ip(&Address, IPAddress.c_str());
    Address.port = Port;

    Host = enet_host_create(&Address, MaxClients, 2, 0, 0);

    if (Host == NULL)
    {
        std::cout << "Error creating server host.\n";
        return;
    }
    printf("ENet server host successfully created.\n");
    printf("Server successfully connected!\n");

    LatestPlayerID = 1;
    Running = true;
}

void Server::PlayerTimeSync(ENetPeer* Peer)
{
    Packet myPacket = {};
    myPacket.type = TIME_SYNC;
    myPacket.timestamp = GetTimeUtils();
    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(Peer, 0, packet);
}

void Server::PlayerCreateCharacter(ENetPeer* Peer)
{
    LatestPlayerID += 1;
    auto* newPlayer = new Player();
    newPlayer->PlayerID = LatestPlayerID;
    Peer->data = newPlayer;
    Players[LatestPlayerID] = Peer;
}

void Server::PlayerJoinNotification(ENetPeer* NewPeer, ENetPeer* PeerToNotify)
{
    Packet myPacket = {};
    myPacket.type = PLAYER_JOIN;
    myPacket.timestamp = GetTimeUtils();

    PlayerJoin playerJoin = {0};
    playerJoin.id = static_cast<Player*>(NewPeer->data)->PlayerID;
    playerJoin.starting_location = {0, 0};
    memcpy(&myPacket.data, &playerJoin, sizeof(playerJoin));

    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(PeerToNotify, 0, packet);
}

void Server::PlayerLeftNotification(ENetPeer* OldPeer, ENetPeer* PeerToNotify)
{
    Packet myPacket;
    myPacket.type = PLAYER_LEFT;
    myPacket.timestamp = GetTimeUtils();

    PlayerLeft left = {0};
    left.id = static_cast<Player*>(OldPeer->data)->PlayerID;
    memcpy(&myPacket.data, &left, sizeof(left));

    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket),
                                            ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(PeerToNotify, 0, packet);
}

void Server::PlayerConnect(ENetEvent& Event)
{
    // Create new player
    printf("Player ID %d joined the game!\n", LatestPlayerID + 1);
    PlayerCreateCharacter(Event.peer);

    // Sending time sync to new player
    PlayerTimeSync(Event.peer);

    auto* NewPlayer = static_cast<Player*>(Event.peer->data);

    for (auto [name,peer] : Players)
    {
        auto* p = static_cast<Player*>(peer->data);
        if (p->PlayerID != NewPlayer->PlayerID)
        {
            // Sending player join to existing player
            PlayerJoinNotification(Event.peer, peer);

            // Sending player join to new player
            PlayerJoinNotification(peer, Event.peer);
        }
    }
}

void Server::PlayerDisconnect(ENetEvent& Event)
{
    auto* OldPlayer = static_cast<Player*>(Event.peer->data);
    printf("Player ID %d has left the game!\n", OldPlayer->PlayerID);
    for (auto& peer : Players | views::values)
    {
        auto* p = static_cast<Player*>(peer->data);
        if (p->PlayerID != OldPlayer->PlayerID)
            PlayerLeftNotification(Event.peer, peer);
    }
    Players.erase(OldPlayer->PlayerID);
    delete OldPlayer;
}

void Server::HandleEvents()
{
    if (!Running)
        return;
    ENetEvent Event;
    int Active = enet_host_service(Host, &Event, 50);
    if (Active > 0)
    {
        switch (Event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            {
                PlayerConnect(Event);
                break;
            }
        case ENET_EVENT_TYPE_RECEIVE:
            {
                if (Event.packet->dataLength != sizeof(Packet))
                {
                    enet_packet_destroy(Event.packet);
                    return;
                }

                Packet packet;

                memcpy(&packet, Event.packet->data, Event.packet->dataLength);

                if (PacketEventActions.contains(packet.type))
                    PacketEventActions[packet.type](*this, packet, Event);

                enet_packet_destroy(Event.packet);
                break;
            }
        case ENET_EVENT_TYPE_DISCONNECT:
            {
                PlayerDisconnect(Event);
                break;
            }
        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}

void Server::HandleTimeSync()
{
    if (!Running)
        return;
    if (GetTimeUtils() - LastSyncedTime >= 1)
    {
        for (auto [id, peer] : Players)
            PlayerTimeSync(peer);
        LastSyncedTime = GetTimeUtils();
    }
}

void Server::HandlePlayerStates()
{
    if (!Running)
        return;
    if (Players.size() <= 1)
        return;

    for (auto [id, peer] : Players)
    {
        for (auto [other_id, other_peer] : Players)
        {
            auto* player = reinterpret_cast<Player*>(other_peer->data);
            if (other_id != id && CompareStates(player->CurrentState, player->LocalState))
                PlayerUpdateNotification(player, peer);
        }
    }

    enet_host_flush(Host);
}

void Server::PlayerUpdateNotification(Player* UpdatedPlayer, ENetPeer* PeerToNotify)
{
    UpdatedPlayer->LocalState = UpdatedPlayer->CurrentState;

    Packet myPacket;
    myPacket.type = PLAYER_UPDATE;
    myPacket.timestamp = UpdatedPlayer->CurrentState.timestamp;

    memcpy(&myPacket.data, &UpdatedPlayer->CurrentState, sizeof(UpdatedPlayer->CurrentState));

    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
    enet_peer_send(PeerToNotify, 0, packet);
}

void Server::UpdateServer()
{
    HandleEvents();
    HandleTimeSync();
    HandlePlayerStates();
}

void Server::StopServer()
{
    if (!Running)
        return;
    if (Host != nullptr)
        enet_host_destroy(Host);
    Reset();
}
