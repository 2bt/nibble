cmake_minimum_required(VERSION 3.10)

project(nibble)

#add_compile_options(-std=c++17 -Wall -O2)
add_compile_options(-std=c++17 -Wall -Og -g)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL REQUIRED
    sdl2
    )

add_executable(nibble
    src/fx.cpp

    src/game/game.cpp
    src/game/render.cpp
    src/game/util.cpp
    )

target_include_directories(nibble PRIVATE
    ${SDL_INCLUDE_DIRS}
    src
    )

target_link_libraries(nibble
    ${SDL_LIBRARIES}
    )
