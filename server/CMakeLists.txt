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
target_link_libraries(c0deckServer PRIVATE httplib nlohmann_json spdlog cli11 SDBusCpp::sdbus-c++)
target_include_directories(c0deckServer PRIVATE .)
# TODO: make optional for optional PulseAudio support
if (EnableAudioStreamControl)
    target_link_libraries(c0deckServer PRIVATE ${PulseAudio_LIBRARIES} ${PulseAudio_MAINLOOP_LIBRARY})
    target_include_directories(c0deckServer PRIVATE ${PulseAudio_INCLUDE_DIRS})
endif()
set_target_properties(c0deckServer PROPERTIES OUTPUT_NAME c0DeckServer)