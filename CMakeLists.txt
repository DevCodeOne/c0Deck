cmake_minimum_required(VERSION 3.14)
project(c0Deck CXX)

option(ENABLE_TESTING "Build Tests" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Quick Gui Qml WebEngine QuickControls2 REQUIRED)
# qt_standard_project_setup(REQUIRES 6.4)

include(FetchContent)
FetchContent_Declare(
        cpp-httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
        GIT_TAG v0.15.3
)

FetchContent_MakeAvailable(cpp-httplib) 

add_subdirectory(src)

if (ENABLE_TESTING)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.14.0
    )

    set(gtest_force_shared_crt_o ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()
