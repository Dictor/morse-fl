cmake_minimum_required(VERSION 3.20.0)
set(BOARD nucleo_h743zi)

find_package(Zephyr)
project(hangang-view)

target_sources(app PRIVATE src/main.c src/app_main.cpp src/hardware.cpp src/task.cpp src/http.cpp)

set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)
#generate_inc_file_for_target(app src/LetEncrypt_R3.der ${gen_dir}/let_encrypt.der.inc)
generate_inc_file_for_target(
    app
    src/globalsign_r1.der
    ${gen_dir}/globalsign_r1.der.inc
    )