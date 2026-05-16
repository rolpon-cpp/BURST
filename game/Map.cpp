//
// Created by lalit on 5/13/2026.
//

#include "../game/Map.h"
#include <random>
#include <iostream>

#include "../network/Utils.h"

#include "Game.h"

using namespace std;

Map::Map()
{
    this->ClearMap();
}

Map::~Map()
{
}

Map::Map(Game* game)
{
    this->game = game;
    this->ClearMap();
}

void Map::GenerateMap(int Seed)
{
    ClearMap();
    std::mt19937 gen(Seed);
    std::uniform_int_distribution distr(0, 3);

    for (int ChunkY = 0; ChunkY < WORLD_CHUNK_SIZE; ChunkY++)
    {
        for (int ChunkX = 0; ChunkX < WORLD_CHUNK_SIZE; ChunkX++)
        {
            Chunk* ThisChunk = GetChunk(ChunkX, ChunkY);
            for (int TileY = 0; TileY < CHUNK_SIZE; TileY++)
            {
                for (int TileX = 0; TileX < CHUNK_SIZE; TileX++)
                {
                    int WorldTileX = (ChunkX * CHUNK_SIZE) + TileX;
                    int WorldTileY = (ChunkY * CHUNK_SIZE) + TileY;
                    char* Tile = GetTileInChunk(ThisChunk, TileX, TileY);
                    if (WorldTileX == 0 || WorldTileY == 0 || WorldTileX == (CHUNK_SIZE*WORLD_CHUNK_SIZE) - 1 || WorldTileY == (CHUNK_SIZE*WORLD_CHUNK_SIZE) - 1)
                    {
                        *Tile = 1;
                        continue;
                    }
                    *Tile = distr(gen) == 2 ? 1 : 0;
                }
            }
        }
    }

}

Chunk* Map::GetChunk(int x, int y)
{
    if (x > WORLD_CHUNK_SIZE || x < 0 || y > WORLD_CHUNK_SIZE || y < 0)
        return nullptr;
    return &Chunks[y * WORLD_CHUNK_SIZE + x];
}

char* Map::GetTileInChunk(Chunk* ChunkToGetFrom, int x, int y)
{
    if (x > CHUNK_SIZE || x < 0 || y > CHUNK_SIZE || y < 0)
        return nullptr;
    return &ChunkToGetFrom->Data[y * CHUNK_SIZE + x];
}

void Map::SetChunk(Chunk* ChunkToSet, int x, int y)
{
    if (x > WORLD_CHUNK_SIZE || x < 0 || y > WORLD_CHUNK_SIZE || y < 0)
        return;
    Chunks[y * WORLD_CHUNK_SIZE + x] = *ChunkToSet;
}

void Map::SetTileInChunk(Chunk* ChunkToSet, char TileToSet, int x, int y)
{
    if (x > CHUNK_SIZE || x < 0 || y > CHUNK_SIZE || y < 0)
        return;
    ChunkToSet->Data[y * CHUNK_SIZE + x] = TileToSet;
}

void Map::ClearMap()
{
    memset(&Chunks, 0, sizeof(Chunks));
    MarkedChunks.clear();
}

bool Map::ChunkIsMarked(int x, int y)
{
    for (MarkedChunk p : MarkedChunks)
    {
        if (p.x == x && p.y == y && GetTimeUtils() - p.timestamp <= 10)
            return true;
    }
    return false;
}

void Map::MarkChunk(int x, int y)
{
    GameClient* gc = (GameClient*)game;
    gc->MainClient.RequestChunk(Vector2{(float)x, (float)y});
    if (ChunkIsMarked(x,y))
        return;
    MarkedChunks.push_back(MarkedChunk{x,y,GetTimeUtils()});
}

void Map::UpdateChunk(int cx, int cy)
{
    Chunk* chunk = GetChunk(cx, cy);
    if (chunk == nullptr)
        return;
    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            char* tile = GetTileInChunk(chunk, x, y);
            if (*tile == 1)
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
        }
    }
}

void Map::Update()
{
    for (int cx = 0; cx < WORLD_CHUNK_SIZE; cx++)
    {
        for (int cy = 0; cy < WORLD_CHUNK_SIZE; cy++)
        {
            Chunk* c = GetChunk(cx, cy);
            if (c != nullptr && !ChunkIsMarked(cx, cy))
                MarkChunk(cx, cy);

            UpdateChunk(cx, cy);

        }
    }
}
