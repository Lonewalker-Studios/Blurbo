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

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
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
    SDL3_image
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image.git"
    GIT_TAG        "main"
)
set(SDLIMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TESTS OFF CACHE BOOL "" FORCE)

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

FetchContent_Declare(
    fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
    GIT_TAG        "10.2.1"
)

FetchContent_Declare(
    soil2
    GIT_REPOSITORY "https://github.com/SpartanJ/SOIL2.git"
    GIT_TAG        "master"
    SOURCE_SUBDIR  "__download_only__"
)

FetchContent_Declare(
    stb
    GIT_REPOSITORY "https://github.com/nothings/stb.git"
    GIT_TAG        "master"
    SOURCE_SUBDIR  "__download_only__"
)

FetchContent_Declare(
    tinyfiledialogs
    GIT_REPOSITORY "https://github.com/native-toolkit/tinyfiledialogs.git"
    GIT_TAG        "master"
    SOURCE_SUBDIR  "__download_only__"
)

FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY "https://github.com/Tencent/rapidjson.git"
    GIT_TAG        "master"
    SOURCE_SUBDIR  "__download_only__"
)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY "https://github.com/madler/zlib.git"
    GIT_TAG        "v1.3.1"
)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    libzip
    GIT_REPOSITORY "https://github.com/nih-at/libzip.git"
    GIT_TAG        "v1.10.1"
)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_REGRESS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    libzippp
    GIT_REPOSITORY "https://github.com/ctrabant/libzippp.git"
    GIT_TAG        "libzippp-v1.0.1-1.10.1"
)
set(LIBZIPPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY "https://github.com/leethomason/tinyxml2.git"
    GIT_TAG        "10.0.0"
)

FetchContent_MakeAvailable(SDL3 SDL3_image box2d entt lua sol2 glm fmt zlib)

if(TARGET zlibstatic)
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
    set(ZLIB_FOUND TRUE CACHE BOOL "" FORCE)
    set(ZLIB_LIBRARIES zlibstatic CACHE STRING "" FORCE)
    set(ZLIB_INCLUDE_DIRS "${zlib_SOURCE_DIR};${zlib_BINARY_DIR}" CACHE STRING "" FORCE)
    set(ZLIB_VERSION_STRING "1.3.1" CACHE STRING "" FORCE)
endif()

FetchContent_MakeAvailable(libzip libzippp tinyxml2)

file(GLOB LUA_SOURCES "${lua_SOURCE_DIR}/*.c")
list(FILTER LUA_SOURCES EXCLUDE REGEX ".*/(lua|luac|onelua|ltests)\\.c$")

add_library(lua SHARED ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${lua_SOURCE_DIR})
if(WIN32)
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

FetchContent_GetProperties(soil2)
if(NOT soil2_POPULATED)
    FetchContent_Populate(soil2)
    file(GLOB SOIL2_SOURCES "${soil2_SOURCE_DIR}/src/SOIL2/*.c")
    add_library(soil2 SHARED ${SOIL2_SOURCES})
    target_include_directories(soil2 PUBLIC ${soil2_SOURCE_DIR}/src/SOIL2)
endif()

FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()

FetchContent_GetProperties(tinyfiledialogs)
if(NOT tinyfiledialogs_POPULATED)
    FetchContent_Populate(tinyfiledialogs)
    add_library(tinyfiledialogs OBJECT ${tinyfiledialogs_SOURCE_DIR}/tinyfiledialogs.c)
    target_include_directories(tinyfiledialogs PUBLIC ${tinyfiledialogs_SOURCE_DIR})
endif()

FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
    add_library(rapidjson INTERFACE)
    target_include_directories(rapidjson INTERFACE ${rapidjson_SOURCE_DIR}/include)
endif()

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
    return 0;
}
]=])
        else()
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
    SDL3_image::SDL3_image
    fmt::fmt
    soil2
    stb
    tinyfiledialogs
    rapidjson
    libzippp::libzippp
    tinyxml2::tinyxml2
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
    SDL3_image::SDL3_image
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
    SDL3_image::SDL3_image
    EnTT::EnTT
    sol2
    box2d
    glm
    fmt::fmt
    soil2
    stb
    tinyfiledialogs
    rapidjson
    libzippp::libzippp
    tinyxml2::tinyxml2
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