cmake_minimum_required(VERSION 3.16)
project(DoomGeneric)

set(CMAKE_C_STANDARD 99)

set(DG_FOLDER "${CMAKE_SOURCE_DIR}/doomgeneric")

file(GLOB SRC_DOOM CONFIGURE_DEPENDS "${DG_FOLDER}/*.c")

list(REMOVE_ITEM SRC_DOOM
    "${DG_FOLDER}/doomgeneric_bare.c"
    "${DG_FOLDER}/doomgeneric_sdl.c")

if (DOOMGENERIC_BARE)
    list(APPEND SRC_DOOM "${DG_FOLDER}/doomgeneric_bare.c")
else()
    list(APPEND SRC_DOOM "${DG_FOLDER}/doomgeneric_sdl.c")
endif()

add_executable(${PROJECT_NAME} ${SRC_DOOM})

find_package(SDL2 QUIET)

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
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_SOURCE_DIRS}/include doomgeneric/)
else()
    message(STATUS "SDL2 found")
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS} doomgeneric/)
endif()

get_filename_component(IWAD_NAME "${CMAKE_SOURCE_DIR}/*.wad" NAME)

set(COMPILE_EXE ${CMAKE_BINARY_DIR}/bin_to_c${CMAKE_EXECUTABLE_SUFFIX})
set(COMPILE_COMMAND ${CMAKE_C_COMPILER} ${CMAKE_SOURCE_DIR}/bin_to_c.c -o ${COMPILE_EXE})
set(RUN_COMMAND ${COMPILE_EXE} ${IWAD_NAME})

add_custom_target(serialize_iwad
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${COMPILE_COMMAND}
    COMMAND ${RUN_COMMAND}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Serializing IWAD file"
)
