cmake_minimum_required(VERSION 3.16)
project(DoomGeneric)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)

file(GLOB SRC_DOOM CONFIGURE_DEPENDS "doomgeneric/*.c")

list(REMOVE_ITEM SRC_DOOM "${CMAKE_SOURCE_DIR}/doomgeneric/doomgeneric_bare.c" "${CMAKE_SOURCE_DIR}/doomgeneric/doomgeneric_sdl.c")

if (DOOMGENERIC_BARE)
    list(APPEND SRC_DOOM "doomgeneric/doomgeneric_bare.c")
else()
    list(APPEND SRC_DOOM "doomgeneric/doomgeneric_sdl.c")
endif()

add_executable(${PROJECT_NAME} ${SRC_DOOM})

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS} doomgeneric/)

target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES} m)
