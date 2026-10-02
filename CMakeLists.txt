cmake_minimum_required(VERSION 3.28)

get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
string(REPLACE " " "_" PROJECT_SAFE_NAME "${PARENT_DIR}")
project(${PROJECT_SAFE_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FETCHCONTENT_QUIET OFF CACHE BOOL "" FORCE)

# Build everything as shared libraries
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Windows: export all symbols from our own DLLs without __declspec annotations
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
    GIT_TAG        "release-3.4.14"
)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY "https://github.com/erincatto/box2d.git"
    GIT_TAG        "v3.1.1"
)
set(BOX2D_SAMPLES OFF CACHE INTERNAL "")
set(BOX2D_BENCHMARKS OFF CACHE INTERNAL "")
set(BOX2D_DOCS OFF CACHE INTERNAL "")
set(BOX2D_UNIT_TESTS OFF CACHE INTERNAL "")

FetchContent_Declare(
    entt
    GIT_REPOSITORY "https://github.com/skypjack/entt.git"
    GIT_TAG        "v4.0.0"
)


FetchContent_Declare(
    lua
    GIT_REPOSITORY "https://github.com/lua/lua.git"
    GIT_TAG        "v5.4.8"
    SOURCE_SUBDIR  "__download_only__"
)

FetchContent_Declare(
    sol2
    GIT_REPOSITORY "https://github.com/ThePhD/sol2.git"
    GIT_TAG        "v3.5.0"
    SOURCE_SUBDIR  "__download_only__"
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm.git"
    GIT_TAG        "1.0.3"
    SOURCE_SUBDIR  "__download_only__"
)

FetchContent_MakeAvailable(SDL3 box2d entt lua sol2 glm)

file(GLOB LUA_SOURCES "${lua_SOURCE_DIR}/*.c")
list(FILTER LUA_SOURCES EXCLUDE REGEX ".*/(lua|luac|onelua|ltests)\\.c$")

add_library(lua SHARED ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${lua_SOURCE_DIR})
if(WIN32)
    # Makes LUA_API expand to dllexport (building lua) / dllimport (consumers)
    target_compile_definitions(lua PUBLIC LUA_BUILD_AS_DLL)
endif()
if(UNIX)
    target_compile_definitions(lua PRIVATE LUA_USE_POSIX)
    target_link_libraries(lua PUBLIC m ${CMAKE_DL_LIBS})
endif()

add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE ${sol2_SOURCE_DIR}/include)
target_link_libraries(sol2 INTERFACE lua)
target_compile_definitions(sol2 INTERFACE SOL_ALL_SAFETIES_ON=1)


add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})

FetchContent_Declare(
    glad
    GIT_REPOSITORY "https://github.com/Dav1dde/glad.git"
    GIT_TAG        "v2.0.8"
    SOURCE_SUBDIR  "cmake"
)
FetchContent_MakeAvailable(glad)

find_package(OpenGL REQUIRED)

set(BLURBO_PROJECTS blurbo_utilities blurbo_window blurbo_editor)

foreach(BLURBO_PROJ IN LISTS BLURBO_PROJECTS)
    set(BLURBO_PROJ_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${BLURBO_PROJ}")

    foreach(BLURBO_SUBDIR src include)
        if(NOT EXISTS "${BLURBO_PROJ_DIR}/${BLURBO_SUBDIR}")
            file(MAKE_DIRECTORY "${BLURBO_PROJ_DIR}/${BLURBO_SUBDIR}")
        endif()
    endforeach()

    file(GLOB_RECURSE BLURBO_EXISTING_SOURCES
        "${BLURBO_PROJ_DIR}/src/*.cpp"
        "${BLURBO_PROJ_DIR}/src/*.c"
    )

    if(NOT BLURBO_EXISTING_SOURCES)
        if(BLURBO_PROJ STREQUAL "blurbo_editor")
            file(WRITE "${BLURBO_PROJ_DIR}/src/main.cpp" [=[
int main(int argc, char** argv)
{
    // code here
    return 0;
}
]=])
        else()
            # A shared library needs at least one exported symbol on Windows,
            # otherwise no import library (.lib) is generated.
            file(WRITE "${BLURBO_PROJ_DIR}/src/${BLURBO_PROJ}.cpp"
                "void ${BLURBO_PROJ}_placeholder() {}\n")
        endif()
    endif()
endforeach()

glad_add_library(GLAD SHARED REPRODUCIBLE API gl:compatibility=4.6)

file(GLOB_RECURSE BLURBO_UTILITIES_SOURCES CONFIGURE_DEPENDS
    "blurbo_utilities/src/*.cpp"
    "blurbo_utilities/src/*.hpp"
    "blurbo_utilities/src/*.c"
    "blurbo_utilities/src/*.h"
    "blurbo_utilities/include/*.h"
    "blurbo_utilities/include/*.hpp"
)

add_library(blurbo_utilities SHARED ${BLURBO_UTILITIES_SOURCES})

target_include_directories(blurbo_utilities PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/blurbo_utilities/include
)

target_link_libraries(blurbo_utilities PUBLIC
    EnTT::EnTT
    sol2
    box2d
    glm
    SDL3::SDL3
)

file(GLOB_RECURSE BLURBO_WINDOW_SOURCES CONFIGURE_DEPENDS
    "blurbo_window/src/*.cpp"
    "blurbo_window/src/*.hpp"
    "blurbo_window/src/*.c"
    "blurbo_window/src/*.h"
    "blurbo_window/include/*.h"
    "blurbo_window/include/*.hpp"
)

add_library(blurbo_window SHARED ${BLURBO_WINDOW_SOURCES})

target_include_directories(blurbo_window PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/blurbo_window/include
)

target_link_libraries(blurbo_window PUBLIC
    blurbo_utilities
    OpenGL::GL
    GLAD
    SDL3::SDL3
)

file(GLOB_RECURSE BLURBO_EDITOR_SOURCES CONFIGURE_DEPENDS
    "blurbo_editor/src/*.cpp"
    "blurbo_editor/src/*.hpp"
    "blurbo_editor/src/*.c"
    "blurbo_editor/src/*.h"
    "blurbo_editor/include/*.h"
    "blurbo_editor/include/*.hpp"
)

add_executable(blurbo_editor ${BLURBO_EDITOR_SOURCES})

target_include_directories(blurbo_editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/blurbo_editor/include
)

target_link_libraries(blurbo_editor PRIVATE
    blurbo_window
    blurbo_utilities
    OpenGL::GL
    GLAD
    SDL3::SDL3
    EnTT::EnTT
    sol2
    box2d
    glm
)

if(WIN32)
    add_custom_command(
        TARGET blurbo_editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:blurbo_editor>
            $<TARGET_FILE_DIR:blurbo_editor>
        COMMAND_EXPAND_LISTS
    )
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT blurbo_editor)
