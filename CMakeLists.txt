cmake_minimum_required(VERSION 3.1)

set(CONFIG_NAME PixelArt)
project(${CONFIG_NAME})
include(CheckCXXCompilerFlag)

# will build a main executable, test programs, a shared and a static library.

# decide if you want to use sfml by recompiling, using shared library or static library
set(DEFAULT_SFML_USE_SOURCE ON)
set(DEFAULT_SFML_STATIC_LIBRARIES ON)
set(DEFAULT_SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML/cmake")
set(DEFAULT_EXTLIBS_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML/extlibs/headers")
set(DEFAULT_EXTLIBS_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML/extlibs/libs-msvc/x64")


set(SFML_USE_SOURCE ${DEFAULT_SFML_USE_SOURCE} CACHE STRING "Use SFML libraries built from sources in deps/SFML")
set(SFML_STATIC_LIBRARIES ${SFML_STATIC_LIBRARIES} CACHE STRING "Use already built SFML libraries in dir SFML_DIR")
set(SFML_DIR ${SFML_DEFAULT_DIR} CACHE STRING "SFML libs location. Should be default ${SFML_DEFAULT_DIR}")
set(EXTLIBS_INCLUDE ${DEFAULT_EXTLIBS_INCLUDE} CACHE STRING "Header location of extlibs used by SFML. Default is ${DEFAULT_EXTLIBS_INCLUDE}")
set(EXTLIBS_LIBS ${DEFAULT_EXTLIBS_LIBS} CACHE STRING "Location of extlibs used by SFML. Default is  ${DEFAULT_EXTLIBS_LIBS}")

if(NOT SFML_USE_SOURCE)
    set(SFML_USE_SOURCE ${DEFAULT_SFML_USE_SOURCE})
endif()
if(NOT SFML_STATIC_LIBRARIES)
    set(SFML_STATIC_LIBRARIES ${DEFAULT_SFML_STATIC_LIBRARIES})
endif()
if(NOT SFML_DIR)
    set(SFML_DIR ${DEFAULT_SFML_DIR})
endif()
if(NOT EXTLIBS_INCLUDE)
    set(EXTLIBS_INCLUDE ${DEFAULT_EXTLIBS_INCLUDE})
endif()
if(NOT EXTLIBS_LIBS)
    set(EXTLIBS_LIBS ${DEFAULT_EXTLIBS_LIBS})
endif()


# If linking against sfml shared libraries, we will copy them to BUILD DIRECTORY.
# Specify their locations and extensions
set(LIB_SFML_GRAPHICS ${SFML_DIR}/lib/${CMAKE_BUILD_TYPE}/sfml-graphics-2.dll)
set(LIB_SFML_WINDOW ${SFML_DIR}/lib/${CMAKE_BUILD_TYPE}/sfml-window-2.dll)
set(LIB_SFML_SYSTEM ${SFML_DIR}/lib/${CMAKE_BUILD_TYPE}/sfml-system-2.dll)

# SFML source files contain extlibs needed to run. ONly relevant for static builds. shared libs will just be loaded from
# OS known shared libs directories
if (NOT SFML_USE_SOURCE)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML/extlibs/headers)   
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML/extlibs/libs-msvc/x64)
endif()

# Include files
set(INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(INCLUDE_SFML_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML/include)
set(INCLUDE_SVG_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/deps/simple-svg/include)
set(INCLUDE_ARGPARSE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/deps/argparse/include)

#sources
set(SOURCE_FILE src/main.cpp)

set(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/voronoi.cpp
)

# specify build tree
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/${CONFIG_NAME})

# compilation options
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++17 Standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wunused-parameter -Wextra -Wreorder -Wconversion -Wsign-conversion")   
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP /bigobj")
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

#we add build targets of the program, executable and libs
set(PIXEL_ART_EXE pixel_art_app)
set(PIXEL_ART_STATIC_LIB pixel_art-s)
set(PIXEL_ART_SHARED_LIB pixel_art)

add_library(${PIXEL_ART_STATIC_LIB} STATIC ${SRCS})
add_library(${PIXEL_ART_SHARED_LIB} SHARED ${SRCS})
add_executable(${PIXEL_ART_EXE} ${SRCS} ${SOURCE_FILE})

# find sfml, build or specify link dir
if(SFML_USE_SOURCE)
    if(SFML_STATIC_LIBRARIES)
        set(BUILD_SHARED_LIBS OFF)
    else()
         set(BUILD_SHARED_LIBS ON)
    endif()
    add_subdirectory("deps/SFML")
else()
    message("SFML_DIR is " ${SFML_DIR})
    find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
endif()

#link
target_link_libraries(${PIXEL_ART_EXE} PRIVATE sfml-graphics sfml-window sfml-system)
target_link_libraries(${PIXEL_ART_STATIC_LIB} PRIVATE sfml-graphics sfml-window sfml-system)
target_link_libraries(${PIXEL_ART_SHARED_LIB} PRIVATE sfml-graphics sfml-window sfml-system)

#include dir
target_include_directories(${PIXEL_ART_EXE}  PRIVATE ${INCLUDE_FOLDER})
target_include_directories(${PIXEL_ART_STATIC_LIB}  PRIVATE ${INCLUDE_FOLDER})
target_include_directories(${PIXEL_ART_SHARED_LIB}  PRIVATE ${INCLUDE_FOLDER})

#include deps

# add simple svg
target_include_directories(${PIXEL_ART_EXE}  PRIVATE ${INCLUDE_SVG_FOLDER})
target_include_directories(${PIXEL_ART_STATIC_LIB}  PRIVATE ${INCLUDE_SVG_FOLDER})
target_include_directories(${PIXEL_ART_SHARED_LIB}  PRIVATE ${INCLUDE_SVG_FOLDER})


# add argparse
target_include_directories(${PIXEL_ART_EXE}  PRIVATE ${INCLUDE_ARGPARSE_FOLDER})
target_include_directories(${PIXEL_ART_STATIC_LIB}  PRIVATE ${INCLUDE_ARGPARSE_FOLDER})
target_include_directories(${PIXEL_ART_SHARED_LIB}  PRIVATE ${INCLUDE_ARGPARSE_FOLDER})

# add sfml
target_include_directories(${PIXEL_ART_EXE}  PRIVATE ${INCLUDE_SFML_FOLDER})
target_include_directories(${PIXEL_ART_STATIC_LIB}  PRIVATE ${INCLUDE_SFML_FOLDER})
target_include_directories(${PIXEL_ART_SHARED_LIB}  PRIVATE ${INCLUDE_SFML_FOLDER})

# Adding tests
add_subdirectory(src/test)

# if linked against shared libs, copying them to PROJECT BINARY DIR so VS can find them, link and run.
#if(NOT SFML_USE_SOURCE AND NOT SFML_STATIC_LIBRARIES)
#add_custom_command(TARGET ${PIXEL_ART_EXE} POST_BUILD COMMAND 
#                   ${CMAKE_COMMAND} -E copy ${LIB_SFML_GRAPHICS} ${LIB_SFML_WINDOW} ${LIB_SFML_SYSTEM} ${PROJECT_BINARY_DIR})
#endif()

#unsetting some cached variables so either user defines them each time, either using default values
unset(SFML_USE_SOURCE CACHE)
unset(SFML_STATIC_LIBRARIES CACHE)
unset(SFML_DIR CACHE)

# done