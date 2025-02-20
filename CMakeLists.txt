cmake_minimum_required(VERSION 3.16)
project(DoomGeneric)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 QUIET)

file(GLOB SRC_DOOM CONFIGURE_DEPENDS "doomgeneric/*.c")

list(REMOVE_ITEM SRC_DOOM "${CMAKE_SOURCE_DIR}/doomgeneric/doomgeneric_bare.c" "${CMAKE_SOURCE_DIR}/doomgeneric/doomgeneric_sdl.c")

if (DOOMGENERIC_BARE)
    list(APPEND SRC_DOOM "doomgeneric/doomgeneric_bare.c")
else()
    list(APPEND SRC_DOOM "doomgeneric/doomgeneric_sdl.c")
endif()

add_executable(${PROJECT_NAME} ${SRC_DOOM})

if (NOT SDL2_FOUND)
    message(STATUS "Fetching SDL2")
    include(FetchContent)
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG SDL2
    )

    FetchContent_MakeAvailable(SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static)
else()
    message(STATUS "SDL2 found")
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
endif()

set(SDL2_INCLUDE_DIRS ${SDL2_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS} doomgeneric/)


