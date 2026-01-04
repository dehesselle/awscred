# SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
#
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.16)

project(
  awscred
  VERSION 0.5
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6)

if(${QT_VERSION} VERSION_LESS 6.10.0)
  message(FATAL_ERROR "You need at least Qt 6.10.0.")
endif()

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools
                                                       GuiPrivate)

set(CMAKE_CXX_FLAGS "-DQT_MESSAGELOGCONTEXT")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp @ONLY)

set(TS_FILES i18n/awscred_de_DE.ts)

set(WINDOWS_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/res/awscred.rc")

set(PROJECT_SOURCES
    src/main.cpp
    src/ProfilesDialog.cpp
    src/ProfilesDialog.hpp
    src/ProfilesDialog.ui
    src/AWSCredentials.cpp
    src/AWSCredentials.hpp
    res/awscred.qrc
    src/IniFile.hpp
    src/IniFile.cpp
    src/version.hpp
    ${TS_FILES}
    ${WINDOWS_RESOURCES})

qt_add_executable(awscred MANUAL_FINALIZATION ${PROJECT_SOURCES})
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(awscred PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(awscred PRIVATE Qt${QT_VERSION_MAJOR}::GuiPrivate)

set_target_properties(awscred PROPERTIES WIN32_EXECUTABLE TRUE)

include(GNUInstallDirs)
install(
  TARGETS awscred
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt_finalize_executable(awscred)
