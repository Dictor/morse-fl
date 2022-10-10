cmake_minimum_required(VERSION 3.20.0)
set(BOARD nucleo_h743zi)

find_package(Zephyr)
project(hangang-view)

target_sources(app PRIVATE src/main.c src/app_main.cpp src/hardware.cpp src/task.cpp)