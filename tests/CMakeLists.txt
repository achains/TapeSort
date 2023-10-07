enable_testing()

add_executable(unit-tests
        main.cpp
)

target_link_libraries(unit-tests PRIVATE GTest::gtest_main)
target_link_libraries(unit-tests PRIVATE tape-tools)

include(GoogleTest)
gtest_discover_tests(unit-tests)
