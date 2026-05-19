//
// Created by lalit on 11/16/2025.
//

#include "../../game_libs.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include "ClientEventActions.h"
#include "Client.h"
#include "../Utils.h"
#include "../Packet.h"
#include "../../game/Map.h"
#include "../../game/Player.h"

using namespace std;

Client::Client(GameClient* game)
{
    this->game = game;
    Reset();
}

Client::Client()
{
    Reset();
}

Client::~Client()
{
}

void Client::Connect(std::string IPAddress, int Port)
{
    if (Connected)
        return;
    Reset();
    printf("Starting client...\n");

    ENetEvent Event;
    ENetAddress Address;

    Host = enet_host_create(NULL, 1, 2, 0, 0);
    if (Host == NULL)
    {
        printf("An error occurred while trying to create an ENet client host.\n");
        return;
    }

    printf("ENet client host successfully created.\n");

    enet_address_set_host(&Address, IPAddress.c_str());
    Address.port = Port;
    Peer = enet_host_connect(Host, &Address, 2, 0);
    if (Peer == NULL)
    {
        printf("No available peers for initiating an ENet connection.\n");
        return;
    }

    printf("Found peer for ENet connection.\n");

    if (enet_host_service(Host, &Event, 5000) > 0 &&
        Event.type == ENET_EVENT_TYPE_CONNECT)
        cout << "Connection to " << IPAddress << ":" << to_string(Port) << " succeeded.\n";
    else
    {
        cout << "Connection to " << IPAddress << ":" << to_string(Port) << " failed.\n";
        return;
    }

    enet_host_bandwidth_limit(Host, 0, 0);

    printf("Client successfully connected!\n");

    Connected = true;
}

void Client::Disconnect()
{
    if (!Connected)
        return;
    uint8_t disconnected = false;

    printf("Disconnecting...\n");

    if (Host != nullptr)
    {
        ENetEvent client_event;
        enet_peer_disconnect(Peer, 0);
        while (enet_host_service(Host, &client_event, 0) > 0 && !disconnected)
        {
            switch (client_event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
                printf("Packet received during disconnection, ignoring packet.\n");
                enet_packet_destroy(client_event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Disconnection succeeded.\n");
                disconnected = true;
                break;
            }
        }
    }

    if (!disconnected)
        printf("Disconnection failed, force disconnection triggered.\n");

    if (Peer != nullptr && !disconnected)
    {
        enet_peer_reset(Peer);
    }

    if (Host != nullptr)
    {
        enet_host_destroy(Host);
        Host = nullptr;
    }

    Peer = nullptr;

    Reset();
}

void Client::Reset()
{
    if (Host != nullptr)
        delete Host;
    if (Peer != nullptr)
        delete Peer;
    EventActions.clear();
    EventActions[TIME_SYNC] = &TimeSyncAction;
    EventActions[PLAYER_JOIN] = &PlayerJoinAction;
    EventActions[PLAYER_LEFT] = &PlayerLeftAction;
    EventActions[PLAYER_UPDATE] = &PlayerUpdateAction;
    EventActions[GET_CHUNK] = &GetChunkAction;
    EventActions[PLAYER_DAMAGE] = &PlayerDamageAction;
    Peer = nullptr;
    Host = nullptr;
    ServerTimeOffset = 0;
    LastUpdatedState = 0;
    Ping = 0;
    OtherPlayers.clear();
    Connected = false;
}

void Client::RequestChunk(Vector2 Position)
{
    if (Host != nullptr && Peer != nullptr)
    {
        Packet myPacket = {};
        myPacket.timestamp = GetServerTime();
        myPacket.type = GET_CHUNK;

        ChunkRequest chunkRequest{};
        chunkRequest.ChunkPos = Position;

        memcpy(&myPacket.data, &chunkRequest, sizeof(ChunkRequest));
        ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
        enet_peer_send(Peer, 0, packet);
        enet_host_flush(Host);
    }
}

void Client::Update()
{
    if (!Connected)
        return;
    ENetEvent Event;
    while (enet_host_service(Host, &Event, 0) > 0)
    {
        switch (Event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            {
                if (Event.packet->dataLength != sizeof(Packet))
                {
                    enet_packet_destroy(Event.packet);
                    return;
                }

                Packet packet;

                memcpy(&packet, Event.packet->data, Event.packet->dataLength);

                if (EventActions.contains(packet.type))
                    EventActions[packet.type](*this, packet, Event);

                enet_packet_destroy(Event.packet);
                break;
            }
        case ENET_EVENT_TYPE_DISCONNECT:
            break;
        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}

std::unordered_map<int32_t, Player>& Client::GetPlayers()
{
    return OtherPlayers;
}

double Client::GetServerTime()
{
    return GetTimeUtils() + ServerTimeOffset;
}

void Client::UpdateState(PlayerState& State)
{
    if (Host != nullptr && Peer != nullptr && GetTimeUtils() - LastUpdatedState >= 1 / 50.0f)
    {
        Packet myPacket = {};
        myPacket.timestamp = State.timestamp;
        myPacket.type = PLAYER_UPDATE;
        memcpy(&myPacket.data, &State, sizeof(PlayerState));
        ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
        enet_peer_send(Peer, 0, packet);
        enet_host_flush(Host);
        LastUpdatedState = GetTimeUtils();
    }
}

void Client::DamagePlayer(long ID, float Damage)
{
    if (Host != nullptr && Peer != nullptr)
    {
        Packet myPacket = {};
        myPacket.timestamp = GetServerTime();
        myPacket.type = PLAYER_DAMAGE;

        PlayerDamage playerDamage{ID, Damage};

        memcpy(&myPacket.data, &playerDamage, sizeof(PlayerDamage));

        ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
        enet_peer_send(Peer, 0, packet);
        enet_host_flush(Host);
    }
}
