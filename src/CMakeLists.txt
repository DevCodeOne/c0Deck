set(SOURCES_LIB
    )

# Qt6
# qt_add_library(c0deckLib STATIC ${SOURCES_LIB})
add_library(c0deckLib STATIC ${SOURCES_LIB})
target_include_directories(c0deckLib PUBLIC .)

set(SOURCES_EXEC
    c0deck.cpp
    animatedbuttonsview.h
    animatedbuttonsview.cpp
    mainwindow.h
    mainwindow.cpp
    
    resources.qrc
)

# Qt6
# qt_add_executable(c0deck
#     ${SOURCES_EXEC})
# 
# qt_add_qml_module(c0deck
#     URI 
#         c0deck
#     QML_FILES
#         qml/MainWindow.qml
#         qml/AnimatedButton.qml
#     RESOURCES
#         ../res/ArrowAnimated.gif
#     )

add_executable(c0deck ${SOURCES_EXEC})

target_link_libraries(c0deck PRIVATE c0deckLib)
target_link_libraries(c0deck PRIVATE httplib)
# target_link_libraries(c0deck PRIVATE Qt6::Core Qt6::Gui Qt6::Quick Qt6::Qml Qt6::WebView Qt6::QuickControls2)
target_link_libraries(c0deck PRIVATE Qt5::Core Qt5::Gui Qt5::Quick Qt5::Qml Qt5::WebEngine Qt5::QuickControls2)

set_target_properties(c0deck PROPERTIES OUTPUT_NAME c0Deck)