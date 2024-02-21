cmake_minimum_required(VERSION 3.14)
project(c0Deck CXX)

option(ENABLE_TESTING "Build Tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(BuildSever "Build server" ON)
option(BuildClient "Build client" ON)
option(EnableAudioStreamControl "Enable audio streams volume control" ON)
option(UseSystem-gRPC "Use system grpc" ON)
option(UseSystem-sdbus-cpp "Use system sdbus cpp" ON)
option(UseSystem-PulseAudio "Use system pulseaudio" ON)

find_package(Qt5 COMPONENTS Core Quick Gui Qml WebEngine QuickControls2 REQUIRED)
# qt_standard_project_setup(REQUIRES 6.4)

include(FetchContent)
FetchContent_Declare(
    cpp-httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
    GIT_TAG v0.15.3
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.13.0
)

FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/cliutils/cli11
    GIT_TAG v2.4.1
)

FetchContent_Declare(
    chaiscript
    GIT_REPOSITORY https://github.com/ChaiScript/ChaiScript/
    GIT_TAG v6.1.0
)

FetchContent_MakeAvailable(cpp-httplib) 
FetchContent_MakeAvailable(nlohmann_json) 
FetchContent_MakeAvailable(spdlog) 
FetchContent_MakeAvailable(CLI11) 

option(BUILD_TESTING "" OFF)
FetchContent_MakeAvailable(chaiscript) 

if (UseSystem-gRPC)
    find_package(gRPC 1.60.0 REQUIRED)
    find_package(protobuf 25.1.0 REQUIRED)
else()
    # TODO: add protobuf with FetchContent_Declare
    set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        gRPC
        GIT_REPOSITORY https://github.com/grpc/grpc
        GIT_TAG v1.60.0
    )

    FetchContent_MakeAvailable(gRPC)
endif()

if (UseSystem-sdbus-cpp)
    find_package(sdbus-c++ REQUIRED)
else()
    # TODO: add FetchContent
endif()

if (EnableAudioStreamControl AND UseSystem-PulseAudio)
    find_package(PulseAudio REQUIRED)
else()
endif()

add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(client)

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
