//
// Created by  on 5/10/2026.
//

#include "Game.h"
#include <iostream>
#include <ostream>
#include "../world/WorldMap.h"
#include "../../network/Utils.h"

Game::Game()
{
    IsClient = false;
    MainMap = WorldMap(this);
    DeltaTime = 0.0f;
    LastTime = this->GetLocalTime();
}

Game::~Game()
{
}

double Game::GetLocalTime()
{
    return GetTimeUtils();
}

double Game::GetServerTime()
{
    return GetTimeUtils();
}

double Game::GetDeltaTime()
{
    return DeltaTime;
}

void Game::Start(string IPAddress, int Port)
{
}

void Game::Stop()
{
    MainMap.ClearMap();
}

void Game::Update()
{
    DeltaTime = this->GetLocalTime() - LastTime;
    LastTime = this->GetLocalTime();
}

void Game::Quit()
{
    Stop();
}
