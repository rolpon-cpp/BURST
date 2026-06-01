cmake_minimum_required(VERSION 3.11)
project(BURST)

set(CMAKE_CXX_STANDARD 26)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
    endif()
endif()

# Dependencies
#find_package(raylib CONFIG REQUIRED)
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
        game/player/Camera.cpp
        game/player/Camera.h
        game/player/Player.cpp
        game/player/Player.h
        network/Utils.cpp
        network/Utils.h
        network/server/ServerEventActions.h
        network/server/ServerEventActions.cpp
        game/world/Map.cpp
        game/world/Map.h
        game_libs.h
        game/UI.cpp
        game/UI.h
        game/player/Weapon.cpp
        game/player/Weapon.h
        game/world/Particles.cpp
        game/world/Particles.h)
target_link_libraries(BURST PRIVATE raylib)
target_link_libraries(BURST PRIVATE unofficial::enet::enet winmm ws2_32)

if (MINGW)
    target_link_options(BouncingPlus PRIVATE
            -static
    )
endif ()