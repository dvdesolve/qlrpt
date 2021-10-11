# This file is part of qlrpt.
#
# qlrpt is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# qlrpt is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with qlrpt. If not, see https://www.gnu.org/licenses/
#
# Author: Viktor Drobot

# global settings
cmake_minimum_required(VERSION 3.12)
project(qlrpt VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Interactive GUI application for receiving, decoding and displaying LRPT images written in C++/Qt")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# use FHS
include(GNUInstallDirs)

# default build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# prevent agressive and unsafe optimizations
# (by default CMake uses -O3 level)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# enable position-independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# build project
add_subdirectory(src)
add_subdirectory(share)
