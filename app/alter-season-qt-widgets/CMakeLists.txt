cmake_minimum_required(VERSION 3.1.0)

project(alter-season-qt-widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED Concurrent REQUIRED)


#if(CMAKE_BUILD_TYPE STREQUAL "Release")
#	set(Torch_DIR "D:/dox/projects/AI/alter-season-cyclegan/app/common/libtorch-1.4-cpu-release/share/cmake/Torch")
#else()
#	set(Torch_DIR "D:/dox/projects/AI/alter-season-cyclegan/app/common/libtorch-1.4-cpu-debug/share/cmake/Torch")
#endif()
#message("Torch dir is " ${Torch_DIR})

find_package(Torch REQUIRED)


#find_package(OpenCV REQUIRED)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")


# TODO: use different targets for windows and linux
add_executable(alter-season-qt-widgets
    conversionfinishedevt.h
    conversionfinishedevt.cpp
    conversionselectordlg.h
    conversionselectordlg.cpp
    converter.h
    converter.cpp
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
        CXX_STANDARD 17
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:alter-season-qt-widgets>"
        )
    #target_set_property(TARGET alter-season-qt-widgets PROPERTY WIN32_EXECUTABLE true)
#endif()

# Copy artifacts
set(ARTIFACTS "${CMAKE_CURRENT_SOURCE_DIR}/../../model/artifacts")
    #"${CMAKE_CURRENT_SOURCE_DIR}/../../model/artifacts/summer_to_winter_torch14_256x256_00100.sm;" 
    #"${CMAKE_CURRENT_SOURCE_DIR}/../../model/artifacts/winter_to_summer_torch14_256x256_00100.sm")

# Copy content of ${ARTIFACTS} directory to target-dir/artifacts directory. If the destination directory does not exist, it will be created. 
add_custom_command(TARGET alter-season-qt-widgets
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${ARTIFACTS}
                    "$<TARGET_FILE_DIR:alter-season-qt-widgets>/artifacts")


# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET alter-season-qt-widgets
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:alter-season-qt-widgets>)

endif (MSVC)


target_link_libraries(alter-season-qt-widgets Qt5::Widgets Qt5::Concurrent)
#target_link_libraries(alter-season-qt-widgets "${OpenCV_LIBS}" "${TORCH_LIBRARIES}")
target_link_libraries(alter-season-qt-widgets "${TORCH_LIBRARIES}")
