//
// Created by lalit on 12/2/2025.
//

#ifndef ALLS_PACKET_H
#define ALLS_PACKET_H

enum PacketType {
    PLAYER_UPDATE,
    PLAYER_DAMAGE,
    PLAYER_JOIN,
    PLAYER_LEFT,
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