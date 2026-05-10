#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include "raylib.h"

#include <iostream>
#include <ranges>

#include "network/client/Client.h"
#include "network/server/Server.h"

using namespace std;

void client() {
    InitWindow(1280, 720, "BURST Client");
    SetTargetFPS(60);

    Player MyPlayer = Player(0, 0, 350.0f);

    Texture2D MyPlayerTexture = LoadTexture("assets/player1.png");
    Texture2D OtherPlayerTexture = LoadTexture("assets/player2.png");

    Client MyClient = Client();

    MyClient.Connect("127.0.0.1", 5000);

    while (!WindowShouldClose()) {
        MyClient.Update();

        BeginDrawing();
        ClearBackground(WHITE);

        MyPlayer.MovePlayer(MyClient.GetServerTime());
        MyClient.UpdateState(MyPlayer.CurrentState);

        for (auto& player : MyClient.GetPlayers() | views::values) {
            player.SmoothPlayerState(MyClient.GetServerTime(), 0.1f);
            DrawTextureEx(OtherPlayerTexture, player.LocalState.position, 0, 0.5f, WHITE);
        }
        DrawTextureEx(MyPlayerTexture, MyPlayer.CurrentState.position, 0, 0.5f, WHITE);

        MyPlayer.LastState = MyPlayer.CurrentState;

        EndDrawing();
    }

    std::cout << "stopping client!" << std::endl;
    MyClient.Disconnect();

    UnloadTexture(MyPlayerTexture);
    UnloadTexture(OtherPlayerTexture);

    CloseWindow();
}

void server() {
    StartServer();
    StopServer();
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