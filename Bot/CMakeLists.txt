cmake_minimum_required(VERSION 3.8)

# ----- Project definition -----

project(Bot)
set(PROJECT_LIB_NAME ${PROJECT_NAME}_Lib)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
file(GLOB_RECURSE PROJECT_HEADERS                           ${INCLUDE_DIR}/*.h)
file(GLOB_RECURSE VENDOR_HEADERS                            ${VENDOR_DIR}/*.h)
set(HEADERS                                                 ${VENDOR_HEADERS} ${PROJECT_HEADERS})

# Source files
file(GLOB_RECURSE VENDOR_IMPL                               ${VENDOR_DIR}/*.cpp)
file(GLOB_RECURSE PROJECT_IMPL                              ${SOURCE_DIR}/*.cpp)
list(FILTER PROJECT_IMPL EXCLUDE REGEX ".*main\\.cpp$")
set(SOURCES                                                 ${VENDOR_IMPL} ${PROJECT_IMPL})

# ----- Binaries building -----
add_library(${PROJECT_LIB_NAME}                     STATIC  ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_LIB_NAME}      PUBLIC  ${VENDOR_DIR} ${INCLUDE_DIR})
target_link_libraries(${PROJECT_LIB_NAME}                   ${LIBRARIES})

add_executable(${PROJECT_NAME}                              ${SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME}               PRIVATE ${PROJECT_LIB_NAME})

# ----- Copying DLLs -----

if (NOT dpp_FOUND AND WIN32)
    include("${CMAKE_SOURCE_DIR}/cmake/CopyDlls.cmake")

    copy_dlls_from_dir("${libdpp_BINARY_DIR}/library" ${PROJECT_NAME})
    copy_dlls(dpp ${PROJECT_NAME})
endif()

# ----- Testing -----

add_subdirectory("tests")

include(CTest)