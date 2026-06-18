//
// Created by  on 6/16/2026.
//

#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "ServerResources.h"
#include "../player/Weapon.h"

using namespace std;

namespace fs = std::filesystem;

ServerResources::ServerResources()
{
}

ServerResources::ServerResources(Game* game)
{
    this->game = game;
}

ServerResources::~ServerResources()
{
}

void ServerResources::Load()
{
     unordered_map<std::string, WeaponData> NewWeapons;
    std::string path = "assets/weapondata";
    for (const auto & entry : fs::directory_iterator(path)) {
        try
        {
            std::string p = entry.path().filename().string();
            p.erase(p.end() - 5, p.end());
            std::ifstream g(entry.path().c_str());
            if (!g.is_open()) {
                std::cout << "WARNING: RESOURCEMANAGER: Failed to open: "
                          << entry.path() << std::endl;
                continue;
            }

            nlohmann::json data = nlohmann::json::parse(g);

            WeaponData wep = {};
            if (data.count("enemies_can_use") && data["enemies_can_use"].get<bool>())
                EnemyWeaponNamesList.push_back(p);

            if (data.count("texture"))
            {
                for (int i = 0; i < 32; i++)
                {
                    wep.texture[i] = 0;
                }
                string d = data["texture"].get<string>().substr(0, min(data["texture"].get<string>().size(), 32));
                memcpy(wep.texture, d.c_str(), d.size());
            }

            if (data.count("type"))
            {
                std::string str_type = data["type"].get<std::string>();
                if (str_type == "none")
                {
                    g.close();
                    continue;
                }
                if (str_type == "projectile")
                {
                    wep.type = PROJECTILE;
                }
            }

            if (data.count("damage"))
                wep.damage = data["damage"].get<float>();
            if (data.count("cooldown"))
                wep.cooldown = data["cooldown"].get<float>();
            if (data.count("range"))
                wep.range = data["range"].get<float>();
            if (data.count("intensity"))
                wep.intensity = data["intensity"].get<float>();
            if (data.count("spreadAngleRange"))
                wep.spreadAngleRange = data["spreadAngleRange"].get<float>();

            if (data.count("shots"))
                wep.shots = data["shots"].get<int>();
            if (data.count("ammo"))
                wep.ammo = data["ammo"].get<int>();

            NewWeapons.insert({p, wep});
            g.close();
        } catch (...)
        {
            cout << "WARNING: WEAPON: Failed to read " << entry << "\n";
        }
    }

    for (auto& [name,value] : NewWeapons)
    {
        Weapons[name] = value;
    }
}

void ServerResources::Unload()
{
    EnemyWeaponNamesList.clear();
    Weapons.clear();
}
