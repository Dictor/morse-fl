cmake_minimum_required(VERSION 3.20.0)
set(BOARD nucleo_h743zi)

find_package(Zephyr)
project(hangang-view)

file(GLOB BEFORE_FONT_SRC src/app_main.cpp src/dns.cpp src/hardware.cpp src/json.cpp src/mqtt.cpp src/task.cpp)
file(GLOB AFTER_FONT_SRC src/custom_font.cpp)
target_sources(app PRIVATE src/main.c  ${BEFORE_FONT_SRC} src/space_grotesk_72.c ${AFTER_FONT_SRC})
zephyr_code_relocate(src/space_grotesk_72.c SRAM2)
set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)