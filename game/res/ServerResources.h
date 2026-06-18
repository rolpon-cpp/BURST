//
// Created by  on 6/16/2026.
//

#ifndef BURST_SERVERRESOURCES_H
#define BURST_SERVERRESOURCES_H


#include "../../game_libs.h"

#include <unordered_map>
#include <string>

struct WeaponData;
using namespace std;

class Game;

class ServerResources
{
public:
    Game* game;

    std::unordered_map<std::string, WeaponData> Weapons;
    std::vector<std::string> EnemyWeaponNamesList;

    ServerResources();
    ServerResources(Game *game);
    ~ServerResources();

    void Load();
    void Unload();
};

#endif //BURST_SERVERRESOURCES_H