//
// Created by lalit on 5/12/2026.
//

#ifndef BURST_CORE_H
#define BURST_CORE_H

#include "Map.h"
#include "../network/server/Server.h"

class Core
{
public:
    Server GameServer;
    Map GameMap;
    bool Running;
    int RenderDistance;

    Core();
    ~Core();

    void Start(string IPAddress = "127.0.0.1", int Port = 5000);
    void Stop();
    void Update();
    void Quit();
};


#endif //BURST_CORE_H