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

find_package(Qt5 5.2 COMPONENTS Widgets REQUIRED)
pkg_check_modules(LIBLRPT REQUIRED liblrpt)


# sources
set(glrpt_SOURCES
    main.cpp
    GlobalObjects.cpp
    IQProcessorWorker.cpp
    IQSourceAbstractWorker.cpp
    IQSourceFileWorker.cpp
    MainWindow.cpp
    SettingsDialog.cpp
    FFTWidget.cpp
    LRPTChanWidget.cpp
    QPSKWidget.cpp
)

set(glrpt_HEADERS
    GlobalDecls.h
    GlobalObjects.h
    IQProcessorWorker.h
    IQSourceAbstractWorker.h
    IQSourceFileWorker.h
    MainWindow.h
    SettingsDialog.h
    FFTWidget.h
    LRPTChanWidget.h
    QPSKWidget.h
)

set(glrpt_FORMS
    MainWindow.ui
    SettingsDialog.ui
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


# where to find external includes
target_include_directories(glrpt SYSTEM PRIVATE ${LIBLRPT_INCLUDE_DIRS})


# where to find external libraries
target_link_directories(glrpt PRIVATE ${LIBLRPT_LIBRARY_DIRS})


# link libraries
target_link_libraries(glrpt PRIVATE Qt5::Widgets)
target_link_libraries(glrpt PRIVATE ${LIBLRPT_LIBRARIES})


# install executable
install(TARGETS glrpt RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
