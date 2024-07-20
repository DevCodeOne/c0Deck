add_executable(
    init
    init.cpp
    testsmain.cpp
)

target_link_libraries(init gtest)

include(GoogleTest)

gtest_discover_tests(init)