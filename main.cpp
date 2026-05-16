
#include "game_libs.h"
#include <iostream>
#include <ranges>

#include "game/Game.h"

using namespace std;

void client() {
    InitWindow(1280, 720, "BURST Client");
    SetTargetFPS(60);

    GameClient game = GameClient();
    game.Start();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        game.Update();

        EndDrawing();
    }

    game.Quit();
    CloseWindow();
}

void server() {
    GameServer game = GameServer();
    game.Start();
    while (game.MainServer.Running)
        game.Update();
    game.Stop();
}

int main(int argc, char** argv) {
    enet_initialize();
    std::string arg = argv[1];
    if (arg== "server") {
        printf("Server selected.\n");
        server();
    } else {
        _sleep(1500);
        printf("Client selected.\n");
        client();
    }
    enet_deinitialize();
    return 0;
}