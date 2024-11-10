cmake_minimum_required (VERSION 3.8)

# ----- Project general definitions -----

# Root directory
get_filename_component(ROOT "${CMAKE_SOURCE_DIR}" ABSOLUTE)

# ----- Solution definition -----

project ("BotPorkassTruffiers")

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D_WINDLL -D_CRT_NONSTDC_NO_WARNINGS)
    add_compile_options(/wd4251) 
endif()

# ----- Subdirectories -----

find_package(dpp QUIET)
if (NOT dpp_FOUND)
    message(STATUS "dpp not found, building it from source")

    set(DPP_BUILD_TEST OFF CACHE BOOL "Disable DPP tests" FORCE)
    set(DPP_INSTALL OFF CACHE BOOL "Disable installation" FORCE)
    set(RUN_LDCONFIG OFF CACHE BOOL "Disable ldconfig" FORCE)
    add_subdirectory("DPP")
else()
    message(STATUS "dpp found")
endif()

add_subdirectory ("Bot")

include(CTest)