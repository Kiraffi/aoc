cmake_minimum_required(VERSION 3.26)
project(aoc2023)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -march=skylake")




add_subdirectory(01)
add_subdirectory(02)
add_subdirectory(03)
add_subdirectory(04)
add_subdirectory(05)
add_subdirectory(06)


add_executable(runall "mainrunner.cpp"
        "01/main.cpp"
        "02/main.cpp"
        "03/main.cpp"
        "04/main.cpp"
        "05/main.cpp"
        "06/main.cpp"
)
target_compile_definitions(runall PRIVATE RUNNER=1 )
