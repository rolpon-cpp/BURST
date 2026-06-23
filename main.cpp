
#include <algorithm>

#include "game_libs.h"
#include <iostream>
#include <random>
#include <sstream>

#include "vector"
#include "game/core/Game.h"
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

    std::string username = "";

    PlayerCustomizedItems customizedItems;

    int prt = 5000;
    std::vector<std::string> c = split(ip, ':');
    if (c.size() == 2)
    {
        ip = c.at(0);
        prt = std::stoi(c.at(1));

        cout << "pls enter username ";
        cin >> username;
    } else
    {
        ip = "127.0.0.1";
        std::vector<std::string> words = {
            "cat", "dog", "bird", "fish", "ant", "bug", "beetle",
            "frog", "duck", "make", "complete", "create",
            "awesome", "what", "epics", "tale", "work",
            "villager", "time", "like", "start", "end",
            "money", "currency", "lisp", "wasp", "done",
            "ocean", "underwater",  "lasting",
            "world", "humans", "animals", "mouth", "love", "happy", "bear", "polarbear",
            "water", "drink", "eat", "consume", "devour", "precious",
            "gem", "diamond", "rock", "canyon", "game", "connection",
            "touch", "feel", "write", "read","fight", "save",
            "purple", "pink", "green", "blue","orange",
        };

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution distr(1000, 9999);

        // Shuffle the entire vector randomly
        std::shuffle(words.begin(), words.end(), gen);

        // Now you can take words sequentially without duplicates
        username = words.at(0);
        username[0] = toupper(username[0]);
        std::string s = words.at(1);
        s[0] = toupper(s[0]);
        username += s;
        username += to_string(distr(gen));
    }

    memcpy(customizedItems.name, username.c_str(), username.size());

    GameClient game = GameClient();
    game.Start(ip, prt, customizedItems);

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