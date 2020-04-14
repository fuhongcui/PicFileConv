cmake_minimum_required(VERSION 3.0)

project(PicFileConv)

set(CMAKE_CXX_STANDARD 11)

include_directories(external/stb_image/include external/trace/include)

aux_source_directory(src MAIN_SRC_FILE)
aux_source_directory(external/stb_image/src MAIN_SRC_FILE)
aux_source_directory(external/trace/src MAIN_SRC_FILE)

add_executable(PicFileConv ${MAIN_SRC_FILE})
