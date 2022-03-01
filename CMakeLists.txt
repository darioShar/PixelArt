cmake_minimum_required(VERSION 3.1)
project(PixelArt)

include(CheckCXXCompilerFlag)

#set(SOURCE_FILE src/test/test_sfml.cpp)
#set(SOURCE_FILE src/test/test_svg.cpp)
#set(SOURCE_FILE src/test/test_graph.cpp)
set(SOURCE_FILE src/main.cpp)

set(INCLUDE_SFML_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/SFML/include)
set(INCLUDE_SVG_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/simple-svg/include)
set(INCLUDE_ARGPARSE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/argparse/include)



if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wunused-parameter -Wextra -Wreorder -Wconversion -Wsign-conversion")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wunused-parameter -Wextra -Wreorder -Wconversion")
    CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP17_FLAG)

    if (HAS_CPP17_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    else()
        message(FATAL_ERROR "Unsupported compiler -- PixelArt requires C++17 support!")
    endif()
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP /bigobj /std:c++17")
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

set(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation_impl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graph.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image.h
    )

#we add the executable of the program

set(PIXEL_ART_TARGET pixel_art)
add_executable(${PIXEL_ART_TARGET} ${SRCS}  ${SOURCE_FILE})

# add simple svg
target_include_directories(pixel_art PRIVATE ${INCLUDE_SVG_FOLDER})

# add argparse
target_include_directories(pixel_art PRIVATE ${INCLUDE_ARGPARSE_FOLDER})

#look for SFML and add it
#include(FetchContent)
#set(BUILD_SHARED_LIBS OFF)
#FetchContent_Declare(
# SFML
# GIT_REPOSITORY https://github.com/SFML/SFML.git
# GIT_TAG 2.5.1
#)
#FetchContent_MakeAvailable(SFML)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory("SFML")
target_link_libraries(pixel_art sfml-graphics sfml-window sfml-system)
target_include_directories(pixel_art PRIVATE ${INCLUDE_SFML_FOLDER})

add_subdirectory(src/test)