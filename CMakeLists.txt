

cmake_minimum_required(VERSION 4.2)

project(fake C)

add_definitions(-DFAKE_FIRMWARE)

add_library(fake
    fake/common.h
    fake/dshot.h
    fake/eeprom.h
    fake/firmwareversion.h
    fake/kiss_telemetry.h
    fake/sounds.h
    fake/version.h
    fake/stubs.c
)

target_include_directories(fake PUBLIC fake)

add_executable(fake_firmware
    Src/main.c
)
target_link_libraries(fake_firmware fake)