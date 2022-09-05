cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(hangang-view)

target_sources(app PRIVATE src/main.c)