
#include "game_libs.h"
#include <iostream>
#include <sstream>

#include "vector"
#include "game/Game.h"
#include "network/Utils.h"

using namespace std;

std::vector<std::string> split(const std::string& s, char delim) {
    std::stringstream ss(s);
    std::string token;
    std::vector<std::string> result;

    while (std::getline(ss, token, delim)) {
        result.push_back(token);
    }
    return result;
}

void client() {
    InitWindow(1280, 720, "BURST Client");
    InitAudioDevice();
    SetTargetFPS(120);

    SetMasterVolume(1.0f);

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

        if (IsKeyPressed(KEY_M))
        {
            if (GetMasterVolume() <= 0.0f)
                SetMasterVolume(1.0f);
            else
                SetMasterVolume(0.0f);
        }

        game.Update();

        EndDrawing();
    }

    game.Quit();
    CloseAudioDevice();
    CloseWindow();
}

void server(int Port) {
    SetRandomSeed(GetTimeUtils());
    GameServer game = GameServer();
    game.Start(Port);
    while (game.MainServer.Running)
        game.Update();
    game.Stop();
}

int main(int argc, char** argv) {
    if (argc < 2)
    {
        cout << "Not enough arguments. Please provide a client/server argument. If server, provide an optional port argument\n" << std::flush;
        return 0;
    }
    enet_initialize();
    std::string arg = argv[1];
    int port = 5000;
    if (argc == 3)
    {
        try
        {
            port = std::stoi(argv[2]);
        } catch (std::invalid_argument& e)
        {
            cout << "failed to get port argument, defaulting to port 5000\n" << std::flush;
        }
    }
    if (arg== "server") {
        printf("Server selected.\n");
        server(port);
    } else {
        _sleep(1500);
        printf("Client selected.\n");
        client();
    }
    enet_deinitialize();
    return 0;
}