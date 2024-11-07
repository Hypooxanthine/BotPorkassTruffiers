cmake_minimum_required(VERSION 3.8)

# ----- Project definition -----

project(Bot)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----- Dependency lib directories -----
find_package(dpp 10.0.35)
if (NOT dpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(dpp
        GIT_REPOSITORY https://github.com/brainboxdotcc/DPP
        GIT_TAG v10.0.35
    )
    FetchContent_MakeAvailable(dpp)
endif()

if (MSVC)
    target_compile_definitions(dpp PUBLIC DPP_STATIC)
endif()

# Grouping the libraries
set(LIBRARIES 
    dpp
)

# ----- Project directories -----

# Project directories
set(SOURCE_DIR  src)
set(INCLUDE_DIR include)
set(VENDOR_DIR  vendor)

# ----- Project files -----

# Header files
file(GLOB_RECURSE PROJECT_HEADERS   ${INCLUDE_DIR}/*.h)
file(GLOB_RECURSE VENDOR_HEADERS    ${VENDOR_DIR}/*.h)
set(HEADERS                         ${VENDOR_HEADERS} ${PROJECT_HEADERS})

# Source files
file(GLOB_RECURSE VENDOR_IMPL   ${VENDOR_DIR}/*.cpp)
file(GLOB_RECURSE PROJECT_IMPL  ${SOURCE_DIR}/*.cpp)
set(SOURCES                     ${VENDOR_IMPL} ${PROJECT_IMPL})

# ----- Binaries building -----
add_executable(Bot                        ${SOURCES} ${HEADERS})
target_include_directories(Bot    PUBLIC  ${VENDOR_DIR} ${INCLUDE_DIR})
target_link_libraries(Bot                 ${LIBRARIES})