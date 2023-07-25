cmake_minimum_required(VERSION 3.8)
project(MyProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# To avoid repeating these options for each target
add_compile_options(-ftrack-macro-expansion=0)

# include directories
include_directories(/usr/local/include/boost)

# link directories
link_directories(/usr/local/lib)

# Specify the executable targets and their dependencies
add_executable(sender sender_main.cpp sender.cpp)
add_executable(receiver receiver_main.cpp)

# Link the necessary libraries
target_link_libraries(sender boost_system ssl crypto)
target_link_libraries(receiver boost_system ssl crypto)

