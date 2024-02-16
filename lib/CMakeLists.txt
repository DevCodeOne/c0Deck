set(SOURCES_LIB
        config.h
        config.cpp
        json_utils.h
    )

# Qt6
# qt_add_library(c0deckLib STATIC ${SOURCES_LIB})
add_library(c0deckLib STATIC ${SOURCES_LIB})
target_link_libraries(c0deckLib PUBLIC nlohmann_json spdlog)
target_include_directories(c0deckLib PUBLIC .)