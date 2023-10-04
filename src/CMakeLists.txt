# Tape Tools Library
set(TAPE_TOOLS_SRC
        tapetools/file_tape.cpp
        tapetools/sort/tape_sort.cpp
        tapetools/sort/block.cpp)

add_library(tape-tools SHARED ${TAPE_TOOLS_SRC})

target_compile_features(tape-tools PRIVATE cxx_std_17)
target_include_directories(
        tape-tools
        SYSTEM
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)


add_executable(tape-sort main.cpp)
target_link_libraries(tape-sort PRIVATE tape-tools)