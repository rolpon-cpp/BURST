//
// Created by lalit on 5/10/2026.
//

#include "Resources.h"
#include <iostream>

using namespace std;

Resources::Resources()
{
}

Resources::~Resources()
{
}

Resources::Resources(Game* game)
{
    this->game = game;
}

void Resources::Load()
{
    Unload();
    FilePathList List = LoadDirectoryFiles(".\\assets");
    for (int i = 0; i < List.count; i++)
    {
        string fn = List.paths[i];
        cout << string(GetFileNameWithoutExt(fn.c_str())) << endl;
        if (fn.ends_with(".png") || fn.ends_with(".jpg") || fn.ends_with(".jpeg"))
            Textures[string(GetFileNameWithoutExt(fn.c_str()))] = LoadTexture(fn.c_str());
    }
}

void Resources::Unload()
{
    for (auto &[name, texture] : Textures)
        UnloadTexture(texture);
    Textures.clear();
}
