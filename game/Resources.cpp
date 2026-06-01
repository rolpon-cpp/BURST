//
// Created by lalit on 5/10/2026.
//

#include "Resources.h"
#include <iostream>

#include "rlgl.h"

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
    DefaultShader = Shader{rlGetShaderIdDefault(), rlGetShaderLocsDefault()};
    Unload();
    FilePathList List = LoadDirectoryFiles(".\\assets");
    for (int i = 0; i < List.count; i++)
    {
        string fn = List.paths[i];
        if (fn.ends_with(".png") || fn.ends_with(".jpg") || fn.ends_with(".jpeg"))
            Textures[string(GetFileNameWithoutExt(fn.c_str()))] = LoadTexture(fn.c_str());
        if (fn.ends_with(".glsl"))
        {
            string fn_without_ext = string(GetFileNameWithoutExt(fn.c_str()));
            bool FragmentShader = true;
            if (fn_without_ext.ends_with("_vert"))
                FragmentShader = false;
            Shader TheShader = LoadShader((FragmentShader ? "" : fn).c_str(), (FragmentShader ? fn : "").c_str());
            if (IsShaderValid(TheShader))
                Shaders[fn_without_ext.substr(0, fn_without_ext.size()-5)] = TheShader;
            else
                cout << "WARNING: FAILED TO LOAD SHADER!!!!!!!!!\n";
        }
    }
}

void Resources::Unload()
{
    for (auto &[name, texture] : Textures)
        UnloadTexture(texture);
    for (auto &[name, shader] : Shaders)
        UnloadShader(shader);
    Textures.clear();
    Shaders.clear();
}

Texture2D& Resources::GetTexture(string texture_name)
{
    if (!Textures.contains(texture_name))
        return Textures["placeholder"];
    return Textures[texture_name];
}

Shader& Resources::GetShader(string shader_name)
{
    if (!Shaders.contains(shader_name))
        return DefaultShader;
    return Shaders[shader_name];
}