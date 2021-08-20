cmake_minimum_required(VERSION 3.10)

project(nibble-games)

add_compile_options(-std=c++17 -Wall -O2)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL REQUIRED
    sdl2
    )

add_executable(nibble-games
    src/fx.cpp

    src/game/game.cpp
    )

target_include_directories(nibble-games PRIVATE
    ${SDL_INCLUDE_DIRS}
    src
    )

target_link_libraries(nibble-games
    ${SDL_LIBRARIES}
    )
