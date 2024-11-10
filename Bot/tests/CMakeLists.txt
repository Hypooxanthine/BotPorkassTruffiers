cmake_minimum_required(VERSION 3.8)

project(BotTests)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES  ./*.cpp)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME}
    GTest::gtest_main
    ${LIBRARIES}
    ${PROJECT_LIB_NAME}
)

enable_testing()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

include(CTest)
