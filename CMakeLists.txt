cmake_minimum_required(VERSION 3.20.0)
set(BOARD nucleo_h743zi)

find_package(Zephyr)
project(morse-fl)

file(GLOB SRC src/app_main.cpp src/dns.cpp src/hardware.cpp src/json.cpp src/mqtt.cpp src/task.cpp src/convolution_layer.cpp src/genann.c src/fullyconnected_layer.cpp)
target_sources(app PRIVATE src/main.c  ${SRC})
set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)