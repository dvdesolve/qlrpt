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

# check for packages
find_package(PkgConfig REQUIRED)

find_package(Qt5 5.2 COMPONENTS Widgets REQUIRED)
pkg_check_modules(LIBLRPT REQUIRED liblrpt)


# sources
set(qlrpt_SOURCES
    main.cpp
    GlobalObjects.cpp
    DecoderWorker.cpp
    DemodulatorWorker.cpp
    IQSourceAbstractWorker.cpp
    IQSourceFileWorker.cpp
    IQSourceSDRWorker.cpp
    QPSKSourceAbstractWorker.cpp
    QPSKSourceFileWorker.cpp
    MainWindow.cpp
    SettingsDialog.cpp
    FFTWidget.cpp
    LRPTChanWidget.cpp
    QPSKWidget.cpp
)

set(qlrpt_HEADERS
    GlobalDecls.h
    GlobalObjects.h
    DecoderWorker.h
    DemodulatorWorker.h
    IQSourceAbstractWorker.h
    IQSourceFileWorker.h
    IQSourceSDRWorker.h
    QPSKSourceAbstractWorker.h
    QPSKSourceFileWorker.h
    MainWindow.h
    SettingsDialog.h
    FFTWidget.h
    LRPTChanWidget.h
    QPSKWidget.h
)

set(qlrpt_FORMS
    MainWindow.ui
    SettingsDialog.ui
)


# directives for Qt-specific stuff
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# primary and the only target
add_executable(qlrpt ${qlrpt_SOURCES} ${qlrpt_HEADERS} ${qlrpt_FORMS})


# compile-time definitions
target_compile_definitions(qlrpt PRIVATE qlrpt_VERSION_FULL="${PROJECT_VERSION}")

# extra compiler options
target_compile_options(qlrpt PRIVATE -Wall -pedantic)


# where to find external includes
target_include_directories(qlrpt SYSTEM PRIVATE ${LIBLRPT_INCLUDE_DIRS})


# where to find external libraries
target_link_directories(qlrpt PRIVATE ${LIBLRPT_LIBRARY_DIRS})


# link libraries
target_link_libraries(qlrpt PRIVATE Qt5::Widgets)
target_link_libraries(qlrpt PRIVATE ${LIBLRPT_LIBRARIES})


# install executable
install(TARGETS qlrpt RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
