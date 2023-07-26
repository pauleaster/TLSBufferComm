# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.8)
project(MyProjectTests VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# To avoid repeating these options for each target
add_compile_options(-ftrack-macro-expansion=0)

# add the Google Test directory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../googletest googletest)


# ${CMAKE_SOURCE_DIR} is ./tests, source in ./src, include in ./include

# include directories
include_directories(${CMAKE_SOURCE_DIR}/../include /usr/local/include/boost)

# link directories
link_directories(/usr/local/lib)

# Specify the executable target for tests
add_executable(tests tests_main.cpp ${CMAKE_SOURCE_DIR}/../src/client.cpp ${CMAKE_SOURCE_DIR}/../src/server.cpp)

# Link the necessary libraries
target_link_libraries(tests gtest gtest_main boost_system ssl crypto)
