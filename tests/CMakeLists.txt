add_executable(
    init
    init.cpp
)

target_link_libraries(
    init
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(init)