#set(SOURCE_FILE src/test/test_sfml.cpp)
#set(SOURCE_FILE src/test/test_svg.cpp)
set(SOURCE_FILE test_sfml.cpp)
#set(SOURCE_FILE src/main.cpp)


#we add the executable of the program

set(TEST_TARGET test_sfml)
add_executable(${TEST_TARGET} ${SOURCE_FILE})

target_link_libraries(test_sfml sfml-graphics sfml-window sfml-system)
target_include_directories(test_sfml PRIVATE ${INCLUDE_SFML_FOLDER})