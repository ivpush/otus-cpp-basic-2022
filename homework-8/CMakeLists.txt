cmake_minimum_required(VERSION 3.5)

project(8.homework)

add_executable(topk_words 
        topk_words.cpp)

target_compile_options(
        topk_words PRIVATE
)

set_target_properties(topk_words PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        )

target_link_libraries(
        topk_words
        PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
)
