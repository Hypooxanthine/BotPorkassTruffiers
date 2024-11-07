cmake_minimum_required (VERSION 3.8)

# ----- Project general definitions -----

# Root directory
get_filename_component(ROOT "${CMAKE_SOURCE_DIR}" ABSOLUTE)

# ----- Solution definition -----

project ("BotPorkassTruffiers")

# ----- Subdirectories -----

add_subdirectory ("Bot")