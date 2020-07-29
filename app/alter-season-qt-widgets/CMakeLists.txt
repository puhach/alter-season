cmake_minimum_required(VERSION 3.1.0)

project(alter-season-qt-widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# TODO: use different targets for windows and linux
add_executable(alter-season-qt-widgets
    imagearea.h
    imagearea.cpp
    #mainwindow.ui
    mainwindow.h
    mainwindow.cpp
    main.cpp
    #resources.qrc
    appicon.rc # https://forum.qt.io/topic/81519/how-to-set-the-application-icon-on-windows-using-cmake
)

# https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
#if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(alter-season-qt-widgets PROPERTIES 
        WIN32_EXECUTABLE true
        CXX_STANDARD 17)
    #target_set_property(TARGET alter-season-qt-widgets PROPERTY WIN32_EXECUTABLE true)
#endif()

target_link_libraries(alter-season-qt-widgets Qt5::Widgets)


