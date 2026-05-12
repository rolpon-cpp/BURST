#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include "raylib.h"

#include <iostream>
#include <ranges>

#include "game/Game.h"
#include "network/client/Client.h"
#include "network/server/Server.h"

using namespace std;

void client() {
    InitWindow(1280, 720, "BURST Client");
    SetTargetFPS(60);

    Game game = Game();
    game.Connect("127.0.0.1", 5000);

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
    Server s = Server();
    s.StartServer();
    while (s.Running)
        s.UpdateServer();
    s.StopServer();
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