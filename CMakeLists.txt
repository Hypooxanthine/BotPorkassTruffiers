cmake_minimum_required (VERSION 3.8)

# ----- Project general definitions -----

# Root directory
get_filename_component(ROOT "${CMAKE_SOURCE_DIR}" ABSOLUTE)

# ----- Solution definition -----

project ("BotPorkassTruffiers")

# ----- Subdirectories -----

set(DPP_BUILD_TEST OFF CACHE BOOL "Disable DPP tests" FORCE)
set(DPP_INSTALL OFF CACHE BOOL "Disable installation" FORCE)
set(RUN_LDCONFIG OFF CACHE BOOL "Disable ldconfig" FORCE)
add_subdirectory("DPP")

add_subdirectory ("Bot")

include(CTest)