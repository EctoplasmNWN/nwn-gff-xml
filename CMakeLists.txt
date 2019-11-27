cmake_minimum_required(VERSION 3.8)
project(gffxml VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

include_directories(external/NWNFileFormats)
add_subdirectory(external/tinyxml2)
add_subdirectory(external/NWNFileFormats)

add_subdirectory(src)
