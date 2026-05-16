//
// Created by lalit on 5/13/2026.
//

#ifndef BURST_MAP_H
#define BURST_MAP_H

#define CHUNK_SIZE 12
#define WORLD_CHUNK_SIZE 2
#define TILE_SIZE 32

#pragma pack(push, 1)
#include <vector>

#include "raymath.h"

struct Chunk
{
    char Data[CHUNK_SIZE * CHUNK_SIZE];
};
#pragma pack(pop)

class Map
{
public:
    Chunk Chunks[WORLD_CHUNK_SIZE * WORLD_CHUNK_SIZE];
    std::vector<Vector3> MarkedChunks;
    Map();
    ~Map();
    void Update();
    void GenerateMap(int Seed);
    Chunk* GetChunk(int x, int y);
    char* GetTileInChunk(Chunk* ChunkToGetFrom, int x, int y);
    void SetChunk(Chunk* ChunkToSet, int x, int y);
    void SetTileInChunk(Chunk* ChunkToSet, char TileToSet, int x, int y);
    void ClearMap();
    bool ChunkIsMarked(int x, int y);
    void MarkChunk(int x, int y);
};

#endif //BURST_MAP_H