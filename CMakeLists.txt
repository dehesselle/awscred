# SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
#
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.19)

project(
  awscred
  VERSION 0.5
  LANGUAGES CXX)

find_package(QT NAMES Qt6)

if(${QT_VERSION} VERSION_LESS 6.10.0)
  message(FATAL_ERROR "You need at least Qt 6.10.0.")
endif()

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools
                                                       GuiPrivate)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES de)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS "-DQT_MESSAGELOGCONTEXT")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp @ONLY)

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
    src/version.hpp)

qt_add_executable(awscred WIN32 ${PROJECT_SOURCES} ${WINDOWS_RESOURCES})

qt_add_translations(TARGETS awscred MERGE_QT_TRANSLATIONS TS_FILES
                    i18n/awscred_de_DE.ts)

target_link_libraries(awscred PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(awscred PRIVATE Qt${QT_VERSION_MAJOR}::GuiPrivate)

include(GNUInstallDirs)

install(
  TARGETS awscred
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
