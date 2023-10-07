enable_testing()

add_executable(unit-tests
        main.cpp
        test_file_tape.cpp
        test_file_tape_sort.cpp
)
target_compile_options(unit-tests PRIVATE -O0 -Wall -Werror)

target_link_libraries(unit-tests PRIVATE GTest::gtest_main)
target_link_libraries(unit-tests PRIVATE tape-tools)

include(GoogleTest)
gtest_discover_tests(unit-tests)
