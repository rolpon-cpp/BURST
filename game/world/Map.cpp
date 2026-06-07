//
// Created by lalit on 5/13/2026.
//

#include "Map.h"
#include <random>
#include <iostream>

#include "../../network/Utils.h"

#include "../Game.h"

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
    //printf("Generating map...\n");
    ClearMap();
    std::mt19937 gen(Seed);
    std::uniform_int_distribution distr(25, 50);
    std::uniform_int_distribution distb(0, 1);
    std::uniform_int_distribution dista(3, 6);
    std::uniform_int_distribution distf(0, (WORLD_CHUNK_SIZE*CHUNK_SIZE) - 1);

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
                        *Tile = 1;
                }
            }
        }
    }

    Vector2 WorldCenter = {
        (CHUNK_SIZE * WORLD_CHUNK_SIZE) / 2.0f,
        (CHUNK_SIZE * WORLD_CHUNK_SIZE) / 2.0f
    };

    int StructureCount = distr(gen);
    for (int i = 0; i < StructureCount; i++)
    {
        int x = distf(gen);
        int y = distf(gen);
        int TileCount = dista(gen);
        for (int r = 0; r < TileCount; r++)
        {
            if (distb(gen) == 1)
                x += distb(gen) == 1 ? 1 : -1;
            else
                y += distb(gen) == 1 ? 1 : -1;
            for (int g = 0; g < 5; g++)
            {
                int fX = x + g * (distb(gen) == 1 ? 1 : -1);
                int fY = y + g * (distb(gen) == 1 ? 1 : -1);
                Vector2 Tile = {(float) fX + 0.5f, (float) fY + 0.5f};
                if (Vector2Distance(WorldCenter, Tile) >= 12)
                    SetTile(1, fX, fY);
            }
        }
    }
    //printf("Map successfully generated!\n");
}

Chunk* Map::GetChunk(int x, int y)
{
    if (x >= WORLD_CHUNK_SIZE || x < 0 || y >= WORLD_CHUNK_SIZE || y < 0)
        return nullptr;
    return &Chunks[y * WORLD_CHUNK_SIZE + x];
}

char* Map::GetTileInChunk(Chunk* ChunkToGetFrom, int x, int y)
{
    if (x >= CHUNK_SIZE || x < 0 || y >= CHUNK_SIZE || y < 0)
        return nullptr;
    return &ChunkToGetFrom->Data[y * CHUNK_SIZE + x];
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
    MarkedChunks.clear();
}

bool Map::ChunkIsMarked(int x, int y)
{
    for (MarkedChunk p : MarkedChunks)
    {
        if (p.x == x && p.y == y && game->GetTime() - p.timestamp <= 10)
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
    MarkedChunks.push_back(MarkedChunk{x,y,game->GetTime()});
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
                float offx = (float)cos((game->GetTime() + ((cx * CHUNK_SIZE) + x)) * 5.0f) * 2.0f;
                float offy = (float)sin((game->GetTime() + ((cy * CHUNK_SIZE) + y)) * 5.0f) * 2.0f;
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
                float theSine = sin((game->GetTime() + x + y) * 0.8f);
                if (theSine >= 0.5f)
                {
                    c.width *= (theSine/6.0f) + 1.0f;
                    c.height *= (theSine/6.0f) + 1.0f;
                    c.x = tileRect.x + tileRect.width/2 - c.width/2;
                    c.y = tileRect.y + tileRect.height/2 - c.height/2;
                }
                DrawRectangleRoundedLinesEx(c,0.1f,2,7.0f,PINK);
                DrawRectangleRounded({c.x + c.width/2 - (c.width-10)/2, c.y + c.height/2 - (c.height-10)/2, c.width - 10, c.height - 10}, 0.1f, 2, ColorAlpha(PINK, 0.4f));
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

            char* tile = GetTile(worldX, worldY);
            if (tile == nullptr || *tile != 1)
                continue;

            Rectangle tileRect = GetTileRect(worldX, worldY);
            if (tileRect.width == 0)
                continue;

            if (CheckCollisionRecs(tileRect, rectangle))
                return true;
        }
    }
    return false;
}

RayCastResult Map::CastRay(Vector2 Origin, float Angle, float Range)
{
    float cX = -cos(Angle * (2 * PI / 360)) * Range;
    float cY = -sin(Angle * (2 * PI / 360)) * Range;
    Vector2 Target = Origin + Vector2(cX, cY);
    return CastRay(Origin, Target, Range);
}

RayCastResult Map::CastRay(Vector2 Origin, Vector2 Target, float Range)
{
    Vector2 vRayStart = Vector2{Origin.x / TILE_SIZE, Origin.y / TILE_SIZE};
    Vector2 vRayTarget = Vector2{Target.x / TILE_SIZE, Target.y / TILE_SIZE};
    Vector2 vRayDir = Vector2Normalize(vRayTarget - vRayStart);

    Vector2 vRayUnitStepSize = { sqrt(1 + (vRayDir.y / vRayDir.x) * (vRayDir.y / vRayDir.x)), sqrt(1 + (vRayDir.x / vRayDir.y) * (vRayDir.x / vRayDir.y)) };
    Vector2i vMapCheck = {(int)vRayStart.x, (int)vRayStart.y};
    Vector2 vRayLength1D = {0, 0};
    Vector2i vStep = {0, 0};

    if (vRayDir.x < 0)
    {
        vStep.x = -1;
        vRayLength1D.x = (vRayStart.x - float(vMapCheck.x)) * vRayUnitStepSize.x;
    }
    else
    {
        vStep.x = 1;
        vRayLength1D.x = (float(vMapCheck.x + 1) - vRayStart.x) * vRayUnitStepSize.x;
    }

    if (vRayDir.y < 0)
    {
        vStep.y = -1;
        vRayLength1D.y = (vRayStart.y - float(vMapCheck.y)) * vRayUnitStepSize.y;
    }
    else
    {
        vStep.y = 1;
        vRayLength1D.y = (float(vMapCheck.y + 1) - vRayStart.y) * vRayUnitStepSize.y;
    }

    if (Range <= 0.0f)
        Range = Vector2Distance(vRayStart, vRayTarget);
    else
        Range /= TILE_SIZE;

    char* tile = nullptr;

    // Perform "Walk" until collision or range check
    bool bTileFound = false;
    float fMaxDistance = Range;
    float fDistance = 0.0f;
    while (!bTileFound && fDistance < fMaxDistance)
    {

        // Walk along shortest path
        if (vRayLength1D.x < vRayLength1D.y)
        {
            vMapCheck.x += vStep.x;
            fDistance = vRayLength1D.x;
            vRayLength1D.x += vRayUnitStepSize.x;
        }
        else
        {
            vMapCheck.y += vStep.y;
            fDistance = vRayLength1D.y;
            vRayLength1D.y += vRayUnitStepSize.y;
        }

        if (fDistance >= fMaxDistance)
        {
            fDistance = fMaxDistance;
            break;
        }

        // Test tile at new test point
        if (vMapCheck.x >= 0 && vMapCheck.x < (CHUNK_SIZE * WORLD_CHUNK_SIZE) && vMapCheck.y >= 0 && vMapCheck.y < (CHUNK_SIZE * WORLD_CHUNK_SIZE))
        {
            char* tileAtMapCheck = GetTile(vMapCheck.x, vMapCheck.y);
            if (tileAtMapCheck != nullptr && *tileAtMapCheck != 0)
            {
                bTileFound = true;
                tile = tileAtMapCheck;
                break;
            }
        }
    }

    // Calculate intersection location
    Vector2 vIntersection = Origin - Vector2Normalize(Origin - Target) * fDistance * TILE_SIZE;

    return {tile, vMapCheck, vIntersection};
}

char* Map::GetTile(int worldX, int worldY)
{
    int x = worldX % CHUNK_SIZE;
    int cx = (worldX - x) / CHUNK_SIZE;
    int y = worldY % CHUNK_SIZE;
    int cy = (worldY - y) / CHUNK_SIZE;

    Chunk* chunk = GetChunk(cx, cy);
    if (chunk == nullptr)
        return nullptr;
    return GetTileInChunk(chunk, x, y);
}

void Map::SetTile(char TileToSet, int worldX, int worldY)
{
    int x = worldX % CHUNK_SIZE;
    int cx = (worldX - x) / CHUNK_SIZE;
    int y = worldY % CHUNK_SIZE;
    int cy = (worldY - y) / CHUNK_SIZE;

    Chunk* chunk = GetChunk(cx, cy);
    if (chunk == nullptr)
        return;
    SetTileInChunk(chunk, TileToSet, x, y);
}
