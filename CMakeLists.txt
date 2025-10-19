cmake_minimum_required(VERSION 3.14)
project(ComputerClub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(club_base STATIC src/ClubLogic.cpp src/Time.cpp)
add_executable(club_app src/main.cpp src/ClubLogic.h src/Time.h src/Models.h)
target_link_libraries(club_app PRIVATE club_base)


include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.16.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(club_tests tests/testClubLogic.cpp tests/testTime.cpp src/ClubLogic.h src/Time.h src/Models.h)
target_link_libraries(club_tests club_base GTest::gtest_main)
target_include_directories(club_tests PRIVATE ${FETCHCONTENT_BASE_DIR}/googletest-src/googletest/include)

include(GoogleTest)
gtest_discover_tests(club_tests)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_custom_target(
    run_file_tests
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_tests.py
    DEPENDS club_app
)
