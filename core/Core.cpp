//
// Created by lalit on 5/12/2026.
//

#include "Core.h"

Core::Core()
{
    GameMap = Map();
    GameServer = Server();
    Running = false;
}

Core::~Core()
{
}

void Core::Start(string IPAddress, int Port)
{
    if (Running)
        return;
    GameMap.GenerateMap(32941);
    GameServer.StartServer(IPAddress, Port);
    Running = true;
}

void Core::Stop()
{
    if (!Running)
        return;
    GameMap.ClearMap();
    GameServer.StopServer();
    Running = false;
}

void Core::Update()
{
    if (!Running)
        return;
    GameServer.UpdateServer();


    if (!GameServer.Running)
        Stop();
}

void Core::Quit()
{
    Running = true;
    Stop();
}
