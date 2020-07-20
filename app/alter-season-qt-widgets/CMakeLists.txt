cmake_minimum_required(VERSION 3.1.0)

project(alter-season-qt-widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(alter-season-qt-widgets
    #mainwindow.ui
    mainwindow.h
    mainwindow.cpp
    main.cpp
    #resources.qrc
)

#if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(alter-season-qt-widgets PROPERTIES WIN32_EXECUTABLE true)
    #target_set_property(TARGET alter-season-qt-widgets PROPERTY WIN32_EXECUTABLE true)
#endif()

target_link_libraries(alter-season-qt-widgets Qt5::Widgets)


