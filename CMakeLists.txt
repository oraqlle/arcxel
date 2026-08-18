cmake_minimum_required(VERSION 3.31)

project(
    arcxel_engine
    VERSION 0.1.0
    DESCRIPTION "Test bench for analysing different threading architectures for game engine workloads."
    LANGUAGES CXX C
)

# ---- Global Defines ----
set(CMAKE_EXPORT_COMPILE_COMMANDS   ON  CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS               ON  CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED     ON  CACHE BOOL "" FORCE)
set(CMAKE_CXX_EXTENSIONS            OFF CACHE BOOL "" FORCE)

# ---- Build Type ----
# Single-config generators leave this empty, which emits no optimisation flags.
# An explicit -DCMAKE_BUILD_TYPE still wins.
get_property(ARCXEL_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if (NOT ARCXEL_MULTI_CONFIG)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
        message(STATUS "arcxel: CMAKE_BUILD_TYPE was unset, defaulting to Release")
    endif ()

    set_property(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel
    )
else ()
    # Multi-config generators choose at build time, so there is no default here.
    message(STATUS "arcxel: multi-config generator, build with --config Release")
endif ()

# ---- raylib Defines ----
set(BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES     OFF CACHE BOOL "" FORCE) # don't build the supplied example games
set(WITH_PIC        ON  CACHE BOOL "" FORCE) # Process Independent Code

# ---- Build Options ----
option(ARXCEL_DEBUG_SANITIZERS "Build with sanitizers enabled" OFF)
option(ARXCEL_LOGGING "Log traces through program" ON)

# ---- Dependencies ----
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY  https://github.com/raysan5/raylib.git
    GIT_TAG         6.0
    GIT_PROGRESS    true
)

FetchContent_MakeAvailable(raylib)

# raylib's headers trip -Wextra in ways we cannot fix, so consumers treat them
# as system headers and stay quiet about them.
set_target_properties(raylib PROPERTIES SYSTEM TRUE)

# ---- Find Packages ----
#find_package(glm 1.0.1 REQUIRED)
#find_package(tinyobjloader 2.0.0 REQUIRED)

# ---- Library ----
# OBJECT, not shared. The timing headers use inline variables that must resolve
# to a single instance across the whole program.
add_library(
    arcxel_lib OBJECT
    src/cube.cxx
    src/engine.cxx
    src/game_object.cxx
    src/log/logger.cxx
    src/object.cxx
    src/player.cxx
    src/scene.cxx
    src/timing/registry.cxx
    src/transform.cxx
    src/window.cxx
)

target_include_directories(arcxel_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(arcxel_lib PUBLIC cxx_std_23)
target_link_libraries(arcxel_lib PUBLIC raylib)

# ---- Warnings ----
if (MSVC)
    set(ARCXEL_WARNINGS /W4)
else ()
    set(ARCXEL_WARNINGS -Wall -Wextra -Wpedantic)
endif ()

target_compile_options(arcxel_lib PRIVATE ${ARCXEL_WARNINGS})

# ---- Entrypoint ----
add_executable(arcxel src/main.cxx)
target_link_libraries(arcxel PRIVATE arcxel_lib)
target_compile_options(arcxel PRIVATE ${ARCXEL_WARNINGS})

# ----  Set Options ----
# Exceptions are still in use: window init throws, and main catches. Re-enable
# -fno-exceptions in E3, once std::expected has replaced both.
#if(MSVC)
#    target_compile_options(arcxel PRIVATE /EHs-c-)
#else()
#    target_compile_options(arcxel PRIVATE -fno-exceptions)
#endif()

# Sanitizers
if(ARXCEL_DEBUG_SANITIZERS)
    if(MSVC)
        target_compile_options(arcxel PRIVATE /fsanitize=address)
        target_link_options(arcxel PRIVATE /fsanitize=address)
    else()
        target_compile_options(arcxel PRIVATE -fsanitize=address)
        target_link_options(arcxel PRIVATE -fsanitize=address)
    endif()
endif()

# ---- Installation ----
#install(TARGETS arcxel RUNTIME COMPONENT runtime)
include(CPack)
