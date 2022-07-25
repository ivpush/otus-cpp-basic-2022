# OTUS C++ basic course homework#3
# CMAKE file for building "guess number" game program

cmake_minimum_required(VERSION 3.8)

project(guess_the_number)

# add_executable(hello hello_world.cpp example.cpp move_me_out.cpp) 
add_executable(
    guess_the_number 
    guess-the-number.cpp
    play-the-game.cpp
    high_scores.cpp
    guess-the-number.h
) 

target_compile_features(guess_the_number PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(guess_the_number PUBLIC /W4 /WX)
else()
    target_compile_options(guess_the_number PUBLIC -Wall -Wextra -Werror -pedantic)
endif()
