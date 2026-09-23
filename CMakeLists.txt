cmake_minimum_required(VERSION 3.31)

project(
    arcxel_engine
    VERSION 0.1.0
    DESCRIPTION "Test bench for analysing different threading architectures for game engine workloads."
    LANGUAGES CXX C
)

# ---- Global Defines ----
set(CMAKE_EXPORT_COMPILE_COMMANDS     ON  CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS                 ON  CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED       ON  CACHE BOOL "" FORCE)
set(CMAKE_CXX_EXTENSIONS              OFF CACHE BOOL "" FORCE)

if(MSVC)
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
else ()
	set(BUILD_SHARED_LIBS ON  CACHE BOOL "" FORCE)
endif ()

# ---- raylib Defines ----
set(BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES     OFF CACHE BOOL "" FORCE) # don't build the supplied example games

# ---- Build Options ----
include(CMakeDependentOption)
cmake_dependent_option(ARCXEL_DEBUG                  "Enable debug options" ON  [[CMAKE_BUILD_TYPE STREQUAL "Debug"]] OFF)
cmake_dependent_option(ARCXEL_SANITIZERS             "Enable sanitizers"    OFF [[CMAKE_BUILD_TYPE STREQUAL "Debug"]] OFF)
cmake_dependent_option(ARCXEL_PHYSICS_DEBUG_RENDERER "Enable sanitizers"    OFF [[CMAKE_BUILD_TYPE STREQUAL "Debug"]] OFF)

option(ARCXEL_LOGGING    "Log program traces"    ON)
option(ARCXEL_PROFILING  "Record profile traces" ON)

# Build options log
message(STATUS "ARXCEL_DEBUG: ${ARCXEL_DEBUG}")
message(STATUS "ARXCEL_SANITIZERS: ${ARCXEL_SANITIZERS}")
message(STATUS "ARXCEL_PHYSICS_DEBUG_RENDERER: ${ARCXEL_PHYSICS_DEBUG_RENDERER}")
message(STATUS "ARXCEL_LOGGING: ${ARCXEL_LOGGING}")
message(STATUS "ARXCEL_PROFILING: ${ARCXEL_PROFILING}")

# ---- Dependencies ----
include(FetchContent)


if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS  ON  CACHE BOOL "" FORCE)
endif()

# -- raylib --
FetchContent_Declare(
    raylib
    GIT_REPOSITORY  https://github.com/raysan5/raylib.git
    GIT_TAG         6.0
    GIT_PROGRESS    TRUE
)

FetchContent_MakeAvailable(raylib)

# raylib's headers trip -Wextra in so consumers treat them as system headers
set_target_properties(raylib PROPERTIES SYSTEM TRUE)

# -- reactphysics3d --
FetchContent_Declare(
    reactphysics3d
    GIT_REPOSITORY  https://github.com/DanielChappuis/reactphysics3d.git
    GIT_TAG         master
    GIT_PROGRESS    TRUE
    GIT_SUBMODULES  ""
)

FetchContent_MakeAvailable(reactphysics3d)

set_target_properties(reactphysics3d PROPERTIES SYSTEM TRUE)

# ---- Exceptions and Warnings ----
if (MSVC)
	set(ARCXEL_NO_EXCEPTIONS /EHs-c-)
    set(ARCXEL_WARNINGS /W4)

    # Remove /EHsc flag to disbale exceptions
    foreach(flag_var 
        CMAKE_CXX_FLAGS 
        CMAKE_CXX_FLAGS_DEBUG 
        CMAKE_CXX_FLAGS_RELEASE 
        CMAKE_CXX_FLAGS_MINSIZEREL 
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
        string(REPLACE "/EHsc" "" ${flag_var} "${${flag_var}}")
    endforeach()

else ()
    set(ARCXEL_NO_EXCEPTIONS -fno-exceptions)
    set(ARCXEL_WARNINGS -Wall -Wextra -Wpedantic)
endif ()

# ---- Library ----
add_library(
    arcxel_lib OBJECT
    src/cube.cxx
    src/engine.cxx
    src/plane.cxx
    src/game_object.cxx
    src/log.cxx
    src/object.cxx
    src/physics.cxx
    src/physics_object.cxx
    src/player.cxx
    src/scene.cxx
    src/sphere.cxx
    src/timing.cxx
    src/transform.cxx
    src/utils.cxx
    src/window.cxx
)

target_include_directories(arcxel_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(arcxel_lib PUBLIC cxx_std_23)
target_compile_options(arcxel_lib PRIVATE ${ARCXEL_WARNINGS} ${ARCXEL_NO_EXCEPTIONS})
target_compile_definitions(arcxel_lib PRIVATE _HAS_EXCEPTIONS=0)
target_compile_definitions(
    arcxel_lib
    PUBLIC
        ARCXEL_DEBUG=$<BOOL:${ARCXEL_DEBUG}>
        ARCXEL_LOGGING=$<BOOL:${ARCXEL_LOGGING}>
        ARCXEL_PROFILING=$<BOOL:${ARCXEL_PROFILING}>
        ARCXEL_PHYSICS_DEBUG_RENDERER=$<BOOL:${ARCXEL_PHYSICS_DEBUG_RENDERER}>
)

target_link_libraries(arcxel_lib PUBLIC raylib ReactPhysics3D::reactphysics3d)

# ---- Entrypoint ----
add_executable(arcxel src/main.cxx)
target_compile_features(arcxel PUBLIC cxx_std_23)
target_compile_options(arcxel PRIVATE ${ARCXEL_WARNINGS} ${ARCXEL_NO_EXCEPTIONS})
target_compile_definitions(arcxel PRIVATE _HAS_EXCEPTIONS=0)
target_link_libraries(arcxel PRIVATE arcxel_lib)

# ----  Enable Sanitizers ----
if(ARCXEL_SANITIZERS)
    if(MSVC)
        set(ARCXEL_SANITIZERS_FLAGS /fsanitize=address)
    else()
        set(ARCXEL_SANITIZERS_FLAGS -fsanitize=address)
    endif()

    target_compile_options(arcxel_lib PRIVATE ${ARCXEL_SANITIZERS_FLAGS})
    target_compile_options(arcxel PRIVATE ${ARCXEL_SANITIZERS_FLAGS})

    target_link_options(arcxel PRIVATE ${ARCXEL_SANITIZERS_FLAGS})
endif()

# ---- Installation ----
#install(TARGETS arcxel RUNTIME COMPONENT runtime)
include(CPack)
