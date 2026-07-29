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

# ---- Headers ----
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---- Entrypoint ----
add_executable(arcxel src/main.cxx)
target_compile_features(arcxel PUBLIC cxx_std_23)
target_link_libraries(arcxel PRIVATE raylib)

# ---- Installation ----
#install(TARGETS arcxel RUNTIME COMPONENT runtime)
include(CPack)
