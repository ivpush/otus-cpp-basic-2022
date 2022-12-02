# book2base - проектная работа для OTUS
#

cmake_minimum_required(VERSION 3.2)

set(CMAKE_TOOLCHAIN_FILE "f:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(book2base)

find_library(sqlite_loc NAMES sqlite3.lib sqlite3 PATHS "f:/dev/vcpkg/installed")

# Main target
add_executable(
    book2base
    book2base-main.cpp
    book_processing.cpp
    book2base.cpp
    database.cpp
    pugixml.cpp
    utils.cpp
)

target_link_libraries(book2base
    PRIVATE
    ${sqlite_loc}
)

set_target_properties(book2base PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(
        book2base PRIVATE
        /W4
    )
else ()
    target_compile_options(
        book2base PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()
