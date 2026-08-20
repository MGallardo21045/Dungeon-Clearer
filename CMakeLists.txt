cmake_minimum_required(VERSION 3.20)

project(DungeonClearer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.0
)

FetchContent_MakeAvailable(SDL3)

add_executable(DungeonClearer
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
)

target_link_libraries(DungeonClearer PRIVATE SDL3::SDL3)

if(WIN32)
    add_custom_command(
        TARGET DungeonClearer POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy
                $<TARGET_FILE:SDL3::SDL3-shared>
                $<TARGET_FILE_DIR:DungeonClearer>
        VERBATIM
    )
endif()