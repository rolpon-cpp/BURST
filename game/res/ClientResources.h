//
// Created by  on 5/10/2026.
//

#ifndef BURST_RESOURCES_H
#define BURST_RESOURCES_H

#include "../../game_libs.h"

#include <unordered_map>
#include <string>

using namespace std;

class Game;

class ClientResources
{
public:
    Game* game;
    unordered_map<string, Texture2D> Textures;
    unordered_map<string, Shader> Shaders;

    Shader DefaultShader;

    ClientResources();
    ClientResources(Game *game);
    ~ClientResources();

    Texture2D& GetTexture(string texture_name);
    Shader& GetShader(string shader_name);

    void Load();
    void Unload();
};


#endif //BURST_RESOURCES_H