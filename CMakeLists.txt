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
endif ()

# ---- raylib Defines ----
set(BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES     OFF CACHE BOOL "" FORCE) # don't build the supplied example games
set(WITH_PIC        ON  CACHE BOOL "" FORCE) # Process Independent Code

# ---- Options ----


# ---- Dependencies ----
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY  https://github.com/raysan5/raylib.git
    GIT_TAG         6.0
    GIT_PROGRESS    true
)

FetchContent_MakeAvailable(raylib)

# ---- Find Packages ----
#find_package(glm 1.0.1 REQUIRED)
#find_package(tinyobjloader 2.0.0 REQUIRED)

# ---- Library ----
# Static, not shared. The timing headers use inline variables that must resolve
# to a single instance across the whole program
add_library(
    arcxel_core STATIC
    src/log/logger.cxx
    src/timing/registry.cxx
    src/triangle.cxx
    src/window.cxx
)
target_include_directories(arcxel_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(arcxel_core PUBLIC cxx_std_23)
target_link_libraries(arcxel_core PUBLIC raylib)

# ---- Entrypoint ----
add_executable(arcxel src/main.cxx)
target_compile_features(arcxel PRIVATE cxx_std_23)
target_link_libraries(arcxel PRIVATE arcxel_core)

# ---- Installation ----
#install(TARGETS arcxel RUNTIME COMPONENT runtime)
include(CPack)
