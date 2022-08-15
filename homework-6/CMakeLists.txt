cmake_minimum_required(VERSION 3.5)

project(containers)

add_executable(containers
               main.cpp)


set_target_properties(
        containers 
    PROPERTIES 
    CXX_STANDARD 17 
    CXX_STANDARD_REQUIRED ON
)
