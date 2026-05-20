
#include "game_libs.h"
#include <iostream>
#include <ranges>

#include "game/Game.h"

using namespace std;

std::vector<std::string> split(const std::string& s, char delim) {
    return s | std::views::split(delim)
             | std::ranges::to<std::vector<std::string>>();
}

void client() {
    InitWindow(1280, 720, "BURST Client");
    SetTargetFPS(120);

    std::string ip = "";
    cout << "pls enter ip:prt ";
    cin >> ip;

    int prt = 5000;
    std::vector<std::string> c = split(ip, ':');
    if (c.size() == 2)
    {
        ip = c.at(0);
        prt = std::stoi(c.at(1));
    } else
    {
        ip = "127.0.0.1";
    }

    GameClient game = GameClient();
    game.Start(ip, prt);

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