add_executable(c0Deck c0deck.cpp)

set(SOURCES_LIB
    mainwindow.cpp)

add_library(c0DeckLib ${SOURCES_LIB})
target_include_directories(c0DeckLib PUBLIC .)

target_link_libraries(c0Deck PRIVATE c0DeckLib)