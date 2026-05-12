//
// Created by lalit on 5/10/2026.
//

#ifndef BURST_RESOURCES_H
#define BURST_RESOURCES_H

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include "raylib.h"

#include <unordered_map>
#include <string>

using namespace std;

class Game;

class Resources
{
public:
    Game* game;
    unordered_map<string, Texture2D> Textures;

    Resources();
    Resources(Game *game);
    ~Resources();

    void Load();
    void Unload();
};


#endif //BURST_RESOURCES_H