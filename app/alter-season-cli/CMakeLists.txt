# CMakeList.txt : CMake project for alter-season-cli, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("alter-season-cli")

cmake_policy(SET CMP0054 NEW)


find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

find_package(OpenCV REQUIRED)


# Add source to this project's executable.
add_executable (alter-season-cli "main.cpp" )

set_property(TARGET alter-season-cli PROPERTY CXX_STANDARD 11)


target_link_libraries(alter-season-cli "${TORCH_LIBRARIES}" "${OpenCV_LIBS}")


# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET alter-season-cli
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:alter-season-cli>)
endif (MSVC)


# TODO: Add tests and install targets if needed.
