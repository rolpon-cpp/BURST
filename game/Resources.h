//
// Created by lalit on 5/10/2026.
//

#ifndef BURST_RESOURCES_H
#define BURST_RESOURCES_H

#include "../game_libs.h"

#include <unordered_map>
#include <string>

using namespace std;

class Game;

class Resources
{
public:
    Game* game;
    unordered_map<string, Texture2D> Textures;
    unordered_map<string, Shader> Shaders;

    Resources();
    Resources(Game *game);
    ~Resources();

    void Load();
    void Unload();
};


#endif //BURST_RESOURCES_H