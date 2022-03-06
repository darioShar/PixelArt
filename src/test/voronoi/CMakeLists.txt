#set(SOURCE_FILE src/test/test_sfml.cpp)
#set(SOURCE_FILE src/test/test_svg.cpp)
set(SOURCE_FILE "test_voronoi_cell.cpp")
#set(SOURCE_FILE src/main.cpp)


#we add the executable of the program

set(TEST_TARGET_CELL test_voronoi_cell)
add_executable(${TEST_TARGET_CELL} ${SRCS} ${SOURCE_FILE})

target_link_libraries(${TEST_TARGET_CELL} sfml-graphics sfml-window sfml-system)
target_include_directories(${TEST_TARGET_CELL} PRIVATE ${INCLUDE_FOLDER})
target_include_directories(${TEST_TARGET_CELL} PRIVATE ${INCLUDE_SFML_FOLDER})

set(SOURCE_FILE "test_voronoi.cpp")

set(TEST_TARGET test_voronoi)
add_executable(${TEST_TARGET} ${SRCS} ${SOURCE_FILE})

target_link_libraries(${TEST_TARGET} sfml-graphics sfml-window sfml-system)
target_include_directories(${TEST_TARGET} PRIVATE ${INCLUDE_FOLDER})
target_include_directories(${TEST_TARGET} PRIVATE ${INCLUDE_SFML_FOLDER})