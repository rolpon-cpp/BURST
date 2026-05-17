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
    std::uniform_int_distribution distr(0, 10);
    Vector2 WorldCenter = {WORLD_CHUNK_SIZE*CHUNK_SIZE / 2.0f, WORLD_CHUNK_SIZE*CHUNK_SIZE / 2.0f};

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
                    if (Vector2Distance(Vector2{(float)WorldTileX, (float)WorldTileY}, WorldCenter) >= 4)
                    {
                        int roll = distr(gen);
                        *Tile = roll >= 8 ? 1 : *Tile;
                        *Tile = roll == 3 ? 2 : *Tile;
                    }
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
            Rectangle tileRect = GetTileRect(cx, cy, x, y);
            if (*tile == 1)
            {
                float sz = TILE_SIZE * 1.125f;
                float offx = (float)cos((GetTimeUtils() + ((cx * CHUNK_SIZE) + x)) * 5.0f) * 2.0f;
                float offy = (float)sin((GetTimeUtils() + ((cy * CHUNK_SIZE) + y)) * 5.0f) * 2.0f;
                DrawRectangleRounded({
                    tileRect.x - (offx/3),
                    tileRect.y - (offy/3),
                    TILE_SIZE,TILE_SIZE
                }, 0.1f, 2, RED);
                DrawRectangleRounded({tileRect.x + tileRect.width/2 - sz/2 + offx,
                    tileRect.y + tileRect.height/2 - sz/2 + offy,
                    sz, sz},
                    0.1f, 2,ColorAlpha(RED, 0.35f));
            } else if (*tile == 2)
            {
                Rectangle c = tileRect;
                float theSine = sin((GetTimeUtils() + x + y) * 0.8f);
                if (theSine >= 0.5f)
                {
                    c.width *= (theSine/6.0f) + 1.0f;
                    c.height *= (theSine/6.0f) + 1.0f;
                    c.x = tileRect.x + tileRect.width/2 - c.width/2;
                    c.y = tileRect.y + tileRect.height/2 - c.height/2;
                }
                DrawRectangleRoundedLinesEx(c,0.1f,2,7.0f,PINK);
            }
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

Rectangle Map::GetTileRect(int cx, int cy, int x, int y)
{
    Chunk* chunk = GetChunk(cx, cy);
    if (chunk == nullptr)
        return {0, 0, 0, 0};
    char* tile = GetTileInChunk(chunk, x, y);
    if (tile == nullptr)
        return {0, 0, 0, 0};
    if (*tile == 0)
        return {0, 0, 0, 0};

    return {((float)(cx * CHUNK_SIZE) + x)*TILE_SIZE, ((float)(cy * CHUNK_SIZE) + y)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
}

Rectangle Map::GetTileRect(int worldX, int worldY)
{
    int x = worldX % CHUNK_SIZE;
    int cx = (worldX - x) / CHUNK_SIZE;
    int y = worldY % CHUNK_SIZE;
    int cy = (worldY - y) / CHUNK_SIZE;
    return GetTileRect(cx, cy, x, y);
}

bool Map::CollisionCheck(Rectangle rectangle)
{
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            int worldX = (rectangle.x / TILE_SIZE) + x;
            int worldY = (rectangle.y / TILE_SIZE) + y;
            Rectangle tileRect = GetTileRect(worldX, worldY);
            if (tileRect.width == 0)
                continue;
            if (CheckCollisionRecs(tileRect, rectangle))
                return true;
        }
    }
    return false;
}
