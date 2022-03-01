#set(SOURCE_FILE src/test/test_sfml.cpp)
#set(SOURCE_FILE src/test/test_svg.cpp)
set(SOURCE_FILE test_graph.cpp)
#set(SOURCE_FILE src/main.cpp)


#we add the executable of the program

set(TEST_TARGET test_graph)
add_executable(${TEST_TARGET} ${SRCS} ${SOURCE_FILE})

target_link_libraries(test_graph sfml-graphics sfml-window sfml-system)
target_include_directories(test_graph PRIVATE ${INCLUDE_SFML_FOLDER})