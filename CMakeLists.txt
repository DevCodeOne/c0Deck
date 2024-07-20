cmake_minimum_required(VERSION 3.6)
project(c0Deck)

option(ENABLE_TESTING "Build Tests" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
cmake_policy(SET CMP0071 NEW)

option(BuildSever "Build server" ON)
option(BuildClient "Build client" ON)
option(EnableAudioStreamControl "Enable audio streams volume control" ON)
option(UseSystem-PulseAudio "Use system pulseaudio" ON)

find_package(Qt5 COMPONENTS Core Quick Gui Qml WebEngine QuickControls2 REQUIRED)
# qt_standard_project_setup(REQUIRES 6.4)

option(BUILD_TESTING "" OFF)

find_package(gRPC CONFIG 1.51.1 REQUIRED)
find_package(spdlog CONFIG 1.14.1 REQUIRED)
find_package(CLI11 CONFIG 2.4.2 REQUIRED)
find_package(httplib CONFIG 0.16.0 REQUIRED)
find_package(sdbus-c++ CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

if (EnableAudioStreamControl AND UseSystem-PulseAudio)
    find_package(ECM REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
    find_package(PulseAudio REQUIRED)
else()
endif()

add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(client)

if (ENABLE_TESTING)
    enable_testing()

    add_subdirectory(tests)
endif()
