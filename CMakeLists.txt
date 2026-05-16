cmake_minimum_required(VERSION 3.11)
project(BURST)

set(CMAKE_CXX_STANDARD 26)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(raylib CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
add_executable(BURST main.cpp
        network/server/Server.cpp
        network/server/Server.h
        network/client/Client.cpp
        network/client/Client.h
        network/Packet.h
        network/client/ClientEventActions.h
        network/client/ClientEventActions.cpp
        game/Game.cpp
        game/Game.h
        game/Resources.cpp
        game/Resources.h
        game/Camera.cpp
        game/Camera.h
        game/Player.cpp
        game/Player.h
        core/Utils.cpp
        core/Utils.h
        network/server/ServerEventActions.h
        network/server/ServerEventActions.cpp
        core/Core.cpp
        core/Core.h
        core/Map.cpp
        core/Map.h)
target_link_libraries(BURST PRIVATE raylib)
target_link_libraries(BURST PRIVATE unofficial::enet::enet winmm ws2_32)