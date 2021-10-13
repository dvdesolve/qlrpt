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

find_package(Qt5 5.14 COMPONENTS Widgets REQUIRED)
pkg_check_modules(LIBLRPT REQUIRED liblrpt)


# sources
set(qlrpt_SOURCES
    dialogs/SettingsDialog.cpp
    qlrpt/main.cpp
    qlrpt/GlobalObjects.cpp
    qlrpt/MainWindow.cpp
    widgets/ConstellationWidget.cpp
    widgets/LRPTChanWidget.cpp
    widgets/WaterfallWidget.cpp
    workers/DecoderWorker.cpp
    workers/DemodulatorWorker.cpp
    workers/IQSourceAbstractWorker.cpp
    workers/IQSourceFileWorker.cpp
    workers/IQSourceSDRWorker.cpp
    workers/QPSKSourceAbstractWorker.cpp
    workers/QPSKSourceFileWorker.cpp
)

set(qlrpt_HEADERS
    dialogs/SettingsDialog.h
    qlrpt/GlobalDecls.h
    qlrpt/GlobalObjects.h
    qlrpt/MainWindow.h
    widgets/ConstellationWidget.h
    widgets/LRPTChanWidget.h
    widgets/WaterfallWidget.h
    workers/DecoderWorker.h
    workers/DemodulatorWorker.h
    workers/IQSourceAbstractWorker.h
    workers/IQSourceFileWorker.h
    workers/IQSourceSDRWorker.h
    workers/QPSKSourceAbstractWorker.h
    workers/QPSKSourceFileWorker.h
)

set(qlrpt_FORMS
    dialogs/SettingsDialog.ui
    qlrpt/MainWindow.ui
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
