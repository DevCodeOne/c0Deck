set(SOURCES_SERVER
    c0deckserver.cpp
    serveractions.h
    serveractions.cpp
    commands.h
    commands.cpp
    audiocontrol.h
    audiocontrol.cpp
    pulsestreams.h
    pulsestreams.cpp
    serverconfig.h
    )

add_executable(c0deckServer ${SOURCES_SERVER})
target_link_libraries(c0deckServer PRIVATE c0deckLib)
target_link_libraries(c0deckServer PRIVATE proto-objects)
target_link_libraries(c0deckServer PRIVATE
        spdlog::spdlog_header_only
        nlohmann_json::nlohmann_json
        CLI11::CLI11
        httplib::httplib
        SDBusCpp::sdbus-c++)

find_path(CHAISCRIPT_INCLUDE_DIR NAMES chaiscript.hpp REQUIRED PATH_SUFFIXES include/chaiscript)

target_include_directories(c0deckServer PRIVATE .)
target_include_directories(c0deckServer PRIVATE
        ${CHAISCRIPT_INCLUDE_DIR}
        ${HTTPLIB_INCLUDE_DIR}
        ${CLI11_INCLUDE_DIR})
# TODO: make optional for optional PulseAudio support
if (EnableAudioStreamControl)
    target_link_libraries(c0deckServer PRIVATE ${PulseAudio_LIBRARIES} ${PulseAudio_MAINLOOP_LIBRARY})
    target_include_directories(c0deckServer PRIVATE ${PulseAudio_INCLUDE_DIRS})
endif()
set_target_properties(c0deckServer PROPERTIES OUTPUT_NAME c0DeckServer)