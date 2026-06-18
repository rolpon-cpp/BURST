//
// Created by  on 5/16/2026.
//

#ifndef BURST_SAFE_ENET_H
#define BURST_SAFE_ENET_H

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include "enet/enet.h"

#include "raylib.h"

struct Vector2i
{
    int x;
    int y;
};

struct RayCastResult
{
    char* hitTile = nullptr;
    Vector2i hitPositionTileSpace;
    Vector2 hitPositionWorldSpace;
};

#endif //BURST_SAFE_ENET_H