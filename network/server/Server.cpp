//
// Created by 7906200 on 11/14/2025.
//

#include "Server.h"
#include <iostream>

#include "../Utils.h"
#include "../../game/player/Player.h"
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
    for (auto &[id,peer] : Players)
        ((Player*)peer->data)->Destroy();
    Players.clear();
    Host = nullptr;
    Running = false;
    LastSyncedTime = 0.0f;
    LastUpdatedPlayers = 0.0f;
    LatestPlayerID = -1;
    PacketEventActions.clear();
    PacketEventActions[PLAYER_UPDATE] = &PlayerUpdateAction;
    PacketEventActions[PLAYER_DASH] = &PlayerDashAction;
    PacketEventActions[GET_CHUNK] = &GetChunkAction;
    PacketEventActions[PLAYER_WEAPON_ATTACK] = &PlayerWeaponAttackAction;
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
    enet_host_bandwidth_limit(Host, 0, 0);
    printf("ENet server host successfully created.\n");
    printf("Server successfully connected!\n");

    LatestPlayerID = 0;
    Running = true;
}

void Server::PlayerTimeSync(ENetPeer* Peer)
{
    Packet myPacket = {};
    myPacket.type = TIME_SYNC;
    myPacket.timestamp = game->GetTime();
    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(Peer, 0, packet);
}

void Server::PlayerCreateCharacter(ENetPeer* Peer)
{
    LatestPlayerID += 1;
    auto* newPlayer = new Player({
        LatestPlayerID, {100 + (float)GetRandomValue(-100, 100), 250
     + (float)GetRandomValue(-100, 100)}, {0, 0}, {0, 0}, 0, 100.0f, 350.0f, WeaponState{}, game->GetTime()
    }, game);
    newPlayer->PlayerID = LatestPlayerID;
    newPlayer->LastState = newPlayer->CurrentState;
    newPlayer->LocalState = newPlayer->CurrentState;

    Peer->data = newPlayer;
    Players[LatestPlayerID] = Peer;

    Packet myPacket = {};
    myPacket.type = PLAYER_CHAR_RESET;
    myPacket.timestamp = game->GetTime();

    PlayerCharacterReset charReset = {0};
    charReset.id = newPlayer->PlayerID;
    charReset.position = newPlayer->CurrentState.position;
    charReset.health = newPlayer->CurrentState.health;
    charReset.timestamp = game->GetTime();
    charReset.speed = newPlayer->CurrentState.speed;

    memcpy(&myPacket.data, &charReset, sizeof(charReset));

    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(Peer, 0, packet);
}

void Server::PlayerJoinNotification(ENetPeer* NewPeer, ENetPeer* PeerToNotify)
{
    Packet myPacket = {};
    myPacket.type = PLAYER_JOIN;
    myPacket.timestamp = game->GetTime();

    PlayerJoin playerJoin = {0};
    playerJoin.id = static_cast<Player*>(NewPeer->data)->PlayerID;
    playerJoin.starting_location = static_cast<Player*>(NewPeer->data)->CurrentState.position;
    memcpy(&myPacket.data, &playerJoin, sizeof(playerJoin));

    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(PeerToNotify, 0, packet);
}

void Server::PlayerLeftNotification(ENetPeer* OldPeer, ENetPeer* PeerToNotify)
{
    Packet myPacket;
    myPacket.type = PLAYER_LEFT;
    myPacket.timestamp = game->GetTime();

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
    for (auto& [id,peer] : Players)
    {
        auto* p = static_cast<Player*>(peer->data);
        if (p->PlayerID != OldPlayer->PlayerID)
            PlayerLeftNotification(Event.peer, peer);
    }
    Players.erase(OldPlayer->PlayerID);
    OldPlayer->Destroy();
    delete OldPlayer;
}

void Server::HandleEvents()
{
    if (!Running)
        return;
    ENetEvent Event;
    while (enet_host_service(Host, &Event, 0) > 0)
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

                if (game->GetTime() - packet.timestamp >= 1.0f)
                    packet.timestamp = game->GetTime();

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
    if (game->GetTime() - LastSyncedTime >= 1)
    {
        for (auto [id, peer] : Players)
            PlayerTimeSync(peer);
        LastSyncedTime = game->GetTime();
    }
}

void Server::HandlePlayerStates()
{
    if (!Running)
        return;
    if (Players.size() <= 1)
        return;
    if (game->GetTime() - LastUpdatedPlayers < 1.0f / 50.0f)
        return;

    for (auto [id, peer] : Players)
    {
        PlayerHealthNotification(peer);
        for (auto [other_id, other_peer] : Players)
        {
            auto* player = reinterpret_cast<Player*>(other_peer->data);
            if (other_id != id)
                PlayerUpdateNotification(player, peer);
        }
    }

    LastUpdatedPlayers = game->GetTime();

    enet_host_flush(Host);
}

void Server::PlayerHealthNotification(ENetPeer* PeerToNotify)
{
    Player* plr = static_cast<Player*>(PeerToNotify->data);
    Packet myPacket;
    myPacket.type = PLAYER_HEALTH_UPDATE;
    myPacket.timestamp = plr->CurrentState.timestamp;

    PlayerHealthUpdate health_update = {plr->CurrentState.health};
    memcpy(&myPacket.data, &health_update, sizeof(health_update));

    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
    enet_peer_send(PeerToNotify, 0, packet);
}

void Server::PlayerUpdateNotification(Player* UpdatedPlayer, ENetPeer* PeerToNotify)
{
    UpdatedPlayer->LocalState = UpdatedPlayer->CurrentState;

    Packet myPacket;
    myPacket.type = PLAYER_UPDATE;
    myPacket.timestamp = UpdatedPlayer->CurrentState.timestamp;

    memcpy(&myPacket.data, &UpdatedPlayer->CurrentState, sizeof(UpdatedPlayer->CurrentState));

    ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
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
