//
// Created by  on 12/2/2025.
//

#ifndef ALLS_PACKET_H
#define ALLS_PACKET_H

enum PacketType {
    PLAYER_WEAPON_ATTACK,
    PLAYER_MOVEMENT_ATTACK,

    PLAYER_UPDATE,
    PLAYER_CHAR_RESET,
    PLAYER_RESPAWN_REQ,
    PLAYER_SERV_PROP_UPDATE,

    PLAYER_JOIN,
    PLAYER_LEFT,
    NEW_PLAYER,

    ANIMATION,
    TIME_SYNC,
    GET_CHUNK,

};

#pragma pack(push, 1)
struct Packet {
    PacketType type;
    double timestamp = -1;
    char data[256];
};
#pragma pack(pop)

#endif //ALLS_PACKET_H