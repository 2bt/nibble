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

    src/app/app.cpp
    src/app/render.cpp
    src/app/util.cpp

    src/app/asteroids/game.cpp
    src/app/snake/game.cpp
    src/app/worm/game.cpp
    )

target_include_directories(nibble PRIVATE
    ${SDL_INCLUDE_DIRS}
    src
    )

target_link_libraries(nibble
    ${SDL_LIBRARIES}
    )
