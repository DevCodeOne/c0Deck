set(SOURCES_SERVER
    c0deckserver.cpp
    )

add_executable(c0deckServer ${SOURCES_SERVER})
target_link_libraries(c0deckServer PRIVATE c0deckLib)
target_link_libraries(c0deckServer PRIVATE proto-objects)
target_link_libraries(c0deckServer PRIVATE httplib nlohmann_json spdlog CLI11::CLI11 SDBusCpp::sdbus-c++)
target_include_directories(c0deckServer PRIVATE .)
# TODO: make optional for optional PulseAudio support
if (EnableAudioStreamControl)
    target_link_libraries(c0deckServer PRIVATE ${PulseAudio_LIBRARIES})
    target_include_directories(c0deckServer PRIVATE ${PulseAudio_INCLUDE_DIRS})
endif()
set_target_properties(c0deckServer PROPERTIES OUTPUT_NAME c0DeckServer)