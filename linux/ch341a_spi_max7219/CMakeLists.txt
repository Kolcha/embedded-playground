# Copyright (c) 2024 Nick Korotysh
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.5)

project(ch341a_spi_max7219 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FindPkgConfig)

pkg_check_modules(LIBUSB REQUIRED IMPORTED_TARGET libusb-1.0)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries (${PROJECT_NAME} PRIVATE PkgConfig::LIBUSB)
