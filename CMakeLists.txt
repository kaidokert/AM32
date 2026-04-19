
cmake_minimum_required(VERSION 3.16)

project(fake C CXX)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DFAKE_FIRMWARE)

option(COVERAGE "Enable code coverage" OFF)
if(COVERAGE)
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
endif()

add_library(fake
    fake/stubs.c
    fake/stubs_noop.c
    Src/functions.c
    Src/kiss_telemetry.c
    Src/signal.c
    Src/dshot.c
)

target_include_directories(fake PUBLIC fake)

add_executable(fake_firmware
    Src/main.c
    fake/stubs_hal.c
)
target_link_libraries(fake_firmware fake)

# --- Tests ---
add_subdirectory(external/Catch2)
add_library(trompeloeil INTERFACE)
target_include_directories(trompeloeil INTERFACE external/trompeloeil/include)

# Testable main.c (no main() symbol)
add_library(am32_main STATIC
    Src/main.c
)
target_link_libraries(am32_main fake)
target_compile_definitions(am32_main PRIVATE AM32_UNIT_TEST)

add_executable(am32_tests
    tests/test_basic.cpp
    tests/test_main.cpp
    tests/test_signal.cpp
    tests/test_dshot.cpp
    tests/test_setinput.cpp
    tests/test_tenkhz.cpp
    tests/test_mainloop.cpp
    tests/test_transfercomplete.cpp
    tests/test_reset.c
    tests/am32_mocks.cpp
)
target_include_directories(am32_tests PRIVATE tests
)
target_link_libraries(am32_tests
    Catch2::Catch2WithMain
    trompeloeil
    am32_main
    fake
)

# --- Black-box test harness ---
add_executable(am32_harness
    fake/harness.c
    fake/stubs_noop.c
    tests/test_reset.c
    Src/main.c
    Src/functions.c
    Src/kiss_telemetry.c
    Src/signal.c
    Src/dshot.c
    fake/stubs.c
)
target_include_directories(am32_harness PRIVATE fake tests)
target_compile_definitions(am32_harness PRIVATE AM32_UNIT_TEST)

if(COVERAGE)
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/am32.profraw
                $<TARGET_FILE:am32_tests>
        COMMAND xcrun llvm-profdata merge -sparse
                ${CMAKE_BINARY_DIR}/am32.profraw
                -o ${CMAKE_BINARY_DIR}/am32.profdata
        COMMAND xcrun llvm-cov report
                $<TARGET_FILE:am32_tests>
                -instr-profile=${CMAKE_BINARY_DIR}/am32.profdata
                -sources ${CMAKE_SOURCE_DIR}/Src/
        COMMAND xcrun llvm-cov show
                $<TARGET_FILE:am32_tests>
                -instr-profile=${CMAKE_BINARY_DIR}/am32.profdata
                -sources ${CMAKE_SOURCE_DIR}/Src/
                -format=html
                -output-dir=${CMAKE_BINARY_DIR}/coverage
        COMMAND ${CMAKE_COMMAND} -E echo "HTML report: ${CMAKE_BINARY_DIR}/coverage/index.html"
        DEPENDS am32_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running tests and generating coverage report"
    )
endif()
