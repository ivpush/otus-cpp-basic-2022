cmake_minimum_required(VERSION 3.2)

project(ProjestWithGTest)

if (MSVC)
    # @see https://github.com/google/googletest/blob/master/googletest/README.md
    # Visual Studio Dynamic vs Static Runtimes
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
# Build googletest
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/googletest" 
    "googletest"
)

# Main target
add_executable(
    demo
    src/main.cpp
    src/containers.hpp
)

# Test target
add_executable(
    containers_tests
    test/list_tests.cpp
    test/arr_tests.cpp
    test/run_tests.cpp
)

# Add src to the include directories for the test target
target_include_directories(
    containers_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Link test target against gtest libraries
target_link_libraries(
    containers_tests PRIVATE
    gtest gtest_main
)

set_target_properties(demo PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
set_target_properties(containers_tests PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(
        demo PRIVATE
        /W4
    )
    target_compile_options(
        containers_tests PRIVATE
        /W4
    )
else ()
    target_compile_options(
        demo PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(
        containers_tests PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()