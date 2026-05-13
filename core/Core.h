//
// Created by lalit on 5/12/2026.
//

#ifndef BURST_CORE_H
#define BURST_CORE_H

#define CHUNK_SIZE 12
#define WORLD_CHUNK_SIZE 16
#include "../network/server/Server.h"

struct Chunk
{
    char Data[CHUNK_SIZE * CHUNK_SIZE];
};

class Map
{
public:
    Chunk Chunks[WORLD_CHUNK_SIZE * WORLD_CHUNK_SIZE];
    Map();
    ~Map();
    void GenerateMap(int Seed);
    Chunk* GetChunk(int x, int y);
    char GetTileInChunk(Chunk* ChunkToGetFrom, int x, int y);
    void SetChunk(Chunk* ChunkToSet, int x, int y);
    void SetTileInChunk(Chunk* ChunkToSet, char TileToSet, int x, int y);
    void ClearMap();
};

class Core
{
public:
    Server GameServer;
    Map GameMap;

    Core();
    ~Core();

    void Start(string IPAddress = "127.0.0.1", int Port = 5000);
    void Stop();
    void Update();
    void Quit();
};


#endif //BURST_CORE_H