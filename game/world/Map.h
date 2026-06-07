//
// Created by lalit on 5/13/2026.
//

#ifndef BURST_MAP_H
#define BURST_MAP_H

#define CHUNK_SIZE 12
#define WORLD_CHUNK_SIZE 4
#define TILE_SIZE 72

#include <vector>
#include "../../game_libs.h"

struct MarkedChunk
{
    int x;
    int y;
    double timestamp;
};

#pragma pack(push, 1)
struct Chunk
{
    char Data[CHUNK_SIZE * CHUNK_SIZE];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ChunkRequest
{
    Vector2 ChunkPos;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ChunkUpload
{
    Vector2 ChunkPos;
    Chunk Chunk;
};
#pragma pack(pop)

class Game;

class Map
{
public:
    Game* game;
    Chunk Chunks[WORLD_CHUNK_SIZE * WORLD_CHUNK_SIZE];
    std::vector<MarkedChunk> MarkedChunks;

    Map();
    Map(Game* game);
    ~Map();

    void Update();
    void UpdateChunk(int cx, int cy);

    void GenerateMap(int Seed);
    void ClearMap();

    Chunk* GetChunk(int x, int y);
    void SetChunk(Chunk* ChunkToSet, int x, int y);
    char* GetTileInChunk(Chunk* ChunkToGetFrom, int x, int y);
    void SetTileInChunk(Chunk* ChunkToSet, char TileToSet, int x, int y);

    char* GetTile(int worldX, int worldY);
    void SetTile(char TileToSet, int worldX, int worldY);

    bool ChunkIsMarked(int x, int y);
    void MarkChunk(int x, int y);

    bool CollisionCheck(Rectangle rectangle);
    RayCastResult CastRay(Vector2 Origin, float Angle, float Range = 1000.0f);
    RayCastResult CastRay(Vector2 Origin, Vector2 Target, float Range = 0.0f);
    Rectangle GetTileRect(int cx, int cy, int x, int y);
    Rectangle GetTileRect(int worldX, int worldY);
};

#endif //BURST_MAP_H