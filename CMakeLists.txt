cmake_minimum_required (VERSION 2.6)
project(matt-mancala)
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 2)
set(MANCALA_TITLE "Mancala")
set(MANCALA_AUTHOR "Matthew Chandler <tardarsauce@gmail.com>")
set(MANCALA_SUMMARY "Kalah game")
set(MANCALA_WEBSITE "http://github.com/mattvchandler/mancala")

set(CMAKE_CXX_FLAGS "-Wall -std=c++0x")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
if(MSYS)
    set(CMAKE_EXE_LINKER_FLAGS "-mwindows")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# libraries
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
find_package(Boost COMPONENTS thread system REQUIRED)

# configure variables
set(bindir ${CMAKE_INSTALL_PREFIX}/bin)
set(datadir ${CMAKE_INSTALL_PREFIX}/share/matt-mancala)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${PROJECT_BINARY_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/matt-mancala.desktop.in
    ${PROJECT_BINARY_DIR}/matt-mancala.desktop)

# directories
include_directories(${GTKMM_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${GTKMM_LINK_DIRS})

# main compilation
add_executable(${PROJECT_NAME}
    appstart.cpp
    board.cpp
    config.cpp
    draw.cpp
    gui.cpp)

target_link_libraries(${PROJECT_NAME}
    ${GTKMM_LIBRARIES}
    ${Boost_LIBRARIES})

# install targets
install(TARGETS "${PROJECT_NAME}" DESTINATION "bin")
install(FILES
    img/bead_blue.png
    img/bead_cyan.png
    img/bead_green.png
    img/bead_magenta.png
    img/bead_red.png
    img/bead_s.png
    img/bead_yellow.png
    img/bg_board.png
    img/bg_bowl.png
    img/bg_store.png
    img/hint.png
    img/icon_128.ico
    img/icon.svg
    DESTINATION "share/matt-mancala/img")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(FILES "img/icon.svg" DESTINATION "share/icons/hicolor/scalable/apps" RENAME "matt-mancala.svg")
    install(FILES "img/icon_128.png" DESTINATION "share/icons/hicolor/128x128/apps" RENAME "matt-mancala.png")
    install(FILES "${PROJECT_BINARY_DIR}/matt-mancala.desktop" DESTINATION "share/applications")
endif()


install(FILES C:/gtkmm340_mingw462/share/glib-2.0/schemas/gschemas.compiled DESTINATION "share/glib-2.0/schemas")
install(FILES
    C:/gtkmm340_mingw462/bin/libatk-1.0-0.dll
    C:/gtkmm340_mingw462/bin/libatkmm-1.6-1.dll
    C:/gtkmm340_mingw462/bin/libcairo-2.dll
    C:/gtkmm340_mingw462/bin/libcairo-gobject-2.dll
    C:/gtkmm340_mingw462/bin/libcairomm-1.0-1.dll
    C:/gtkmm340_mingw462/bin/libffi-5.dll
    C:/gtkmm340_mingw462/bin/libfontconfig-1.dll
    C:/gtkmm340_mingw462/bin/libfreetype-6.dll
    C:/gtkmm340_mingw462/bin/libgcc_s_dw2-1.dll
    C:/gtkmm340_mingw462/bin/libgdk-3-0.dll
    C:/gtkmm340_mingw462/bin/libgdk_pixbuf-2.0-0.dll
    C:/gtkmm340_mingw462/bin/libgdkmm-3.0-1.dll
    C:/gtkmm340_mingw462/bin/libgio-2.0-0.dll
    C:/gtkmm340_mingw462/bin/libgiomm-2.4-1.dll
    C:/gtkmm340_mingw462/bin/libglib-2.0-0.dll
    C:/gtkmm340_mingw462/bin/libglibmm-2.4-1.dll
    C:/gtkmm340_mingw462/bin/libgmodule-2.0-0.dll
    C:/gtkmm340_mingw462/bin/libgobject-2.0-0.dll
    C:/gtkmm340_mingw462/bin/libgtk-3-0.dll
    C:/gtkmm340_mingw462/bin/libgtkmm-3.0-1.dll
    C:/gtkmm340_mingw462/bin/libjasper-1.dll
    C:/gtkmm340_mingw462/bin/libjpeg-8.dll
    C:/gtkmm340_mingw462/bin/liblzma-5.dll
    C:/gtkmm340_mingw462/bin/libpango-1.0-0.dll
    C:/gtkmm340_mingw462/bin/libpangocairo-1.0-0.dll
    C:/gtkmm340_mingw462/bin/libpangoft2-1.0-0.dll
    C:/gtkmm340_mingw462/bin/libpangomm-1.4-1.dll
    C:/gtkmm340_mingw462/bin/libpangowin32-1.0-0.dll
    C:/gtkmm340_mingw462/bin/libpixman-1-0.dll
    C:/gtkmm340_mingw462/bin/libpng15-15.dll
    C:/gtkmm340_mingw462/bin/libsigc-2.0-0.dll
    C:/gtkmm340_mingw462/bin/libstdc++-6.dll
    C:/gtkmm340_mingw462/bin/libtiff-5.dll
    C:/gtkmm340_mingw462/bin/libxml2-2.dll
    C:/gtkmm340_mingw462/bin/zlib1.dll
    C:/gtkmm340_mingw462/lib/libboost_chrono-mgw46-mt-1_55.dll
    C:/gtkmm340_mingw462/lib/libboost_system-mgw46-mt-1_55.dll
    C:/gtkmm340_mingw462/lib/libboost_thread-mgw46-mt-1_55.dll
    DESTINATION "bin/")

# CPack vars
set(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_CONTACT "${MANCALA_AUTHOR}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY)
set(CPACK_INSTALL_CMAKE_PROJECTS
    "${CMAKE_BINARY_DIR};${PROJECT_NAME};ALL;/")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${MANCALA_TITLE}: ${MANCALA_SUMMARY}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
set(CPACK_PACKAGE_FILE_README "${CMAKE_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")
set(CPACK_PACKAGE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_EXECUTABLES "mancala")

# Debian-specific settings
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_program(DPKG "dpkg")
    if(DPKG)
        set(CPACK_GENERATOR DEB;TGZ)
        execute_process(COMMAND ${DPKG} --print-architecture
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS
            "libgtkmm-3.0-1")
        set(CPACK_DEBIAN_PACKAGE_SECTION "games")
        string(TOLOWER "${PROJECT_NAME}" PACKAGE_NAME_LOWER)
        set(CPACK_PACKAGE_FILE_NAME
            "${PACKAGE_NAME_LOWER}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
    endif()
endif()

# Windows NSIS specifics:
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CPACK_GENERATOR NSIS;TGZ)
    set(CPACK_PACKAGE_EXECUTABLES "matt-mancala;Mancala")
    set(CPACK_NSIS_DISPLAY_NAME "Mancala")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "matt-mancala")
    set(CPACK_NSIS_CONTACT "${MANCALA_AUTHOR}")
    set(CPACK_NSIS_HELP_LINK "${MANCALA_WEBSITE}")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/img/icon_128.ico")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
endif()

include(CPack)
