# This file is part of glrpt.
#
# glrpt is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# glrpt is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with glrpt. If not, see https://www.gnu.org/licenses/
#
# Author: Viktor Drobot

# check for packages
find_package(PkgConfig REQUIRED)

find_package(Qt5 COMPONENTS Widgets REQUIRED)


# sources
set(glrpt_SOURCES
    main.cpp
    FFTWidget.cpp
    LRPTChanWidget.cpp
    MainWindow.cpp
    QPSKWidget.cpp
)

set(glrpt_HEADERS
    FFTWidget.h
    LRPTChanWidget.h
    MainWindow.h
    QPSKWidget.h
)

set(glrpt_FORMS
    MainWindow.ui
)


# directives for Qt-specific stuff
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# primary and the only target
add_executable(glrpt ${glrpt_SOURCES} ${glrpt_HEADERS} ${glrpt_FORMS})


# extra compiler options
target_compile_options(glrpt PRIVATE -Wall -pedantic)


# link libraries
target_link_libraries(glrpt PRIVATE Qt5::Widgets)


# install executable
install(TARGETS glrpt RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
