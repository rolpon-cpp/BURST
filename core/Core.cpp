//
// Created by lalit on 5/12/2026.
//

#include "Core.h"

Map::Map()
{
}

Map::~Map()
{
}

void Map::GenerateMap(int Seed)
{
}

Chunk* Map::GetChunk(int x, int y)
{
    if (x >= WORLD_CHUNK_SIZE || x < 0 || y >= WORLD_CHUNK_SIZE || y < 0)
        return nullptr;
    return &Chunks[y * WORLD_CHUNK_SIZE + x];
}

char Map::GetTileInChunk(Chunk* ChunkToGetFrom, int x, int y)
{
    if (x >= CHUNK_SIZE || x < 0 || y >= CHUNK_SIZE || y < 0)
        return 0;
    return ChunkToGetFrom->Data[y * CHUNK_SIZE + x];
}

void Map::SetChunk(Chunk* ChunkToSet, int x, int y)
{
    if (x >= WORLD_CHUNK_SIZE || x < 0 || y >= WORLD_CHUNK_SIZE || y < 0)
        return;
    Chunks[y * WORLD_CHUNK_SIZE + x] = *ChunkToSet;
}

void Map::SetTileInChunk(Chunk* ChunkToSet, char TileToSet, int x, int y)
{
    if (x >= CHUNK_SIZE || x < 0 || y >= CHUNK_SIZE || y < 0)
        return;
    ChunkToSet->Data[y * CHUNK_SIZE + x] = TileToSet;
}

void Map::ClearMap()
{
    memset(&Chunks, 0, sizeof(Chunks));
}

Core::Core()
{
    GameMap = Map();
    GameServer = Server();
}

Core::~Core()
{
}

void Core::Start(string IPAddress, int Port)
{
}

void Core::Stop()
{
    GameMap.ClearMap();
    GameServer.StopServer();
}

void Core::Update()
{
}

void Core::Quit()
{
    Stop();

}
