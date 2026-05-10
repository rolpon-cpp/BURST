//
// Created by lalit on 5/10/2026.
//

#include "Resources.h"

Resources::Resources()
{
}

Resources::~Resources()
{
}

void Resources::Load()
{
    FilePathList List = LoadDirectoryFiles("../assets");
    for (int i = 0; i < List.count; i++)
    {
        string fn = List.paths[i];
        if (fn.ends_with(".png") || fn.ends_with(".jpg") || fn.ends_with(".jpeg"))
            Textures[fn] = LoadTexture(("../assets/" + fn).c_str());
    }
}

void Resources::Unload()
{
    for (auto &[name, texture] : Textures)
        UnloadTexture(texture);
}
