cmake_minimum_required(VERSION 3.26)
project(aoc2023)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -march=skylake -Wall -Wextra -pedantic")

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)


add_executable(01exec "01/main.cpp")
add_executable(02exec "02/main.cpp")
add_executable(03exec "03/main.cpp")
add_executable(04exec "04/main.cpp")
add_executable(05exec "05/main.cpp")
add_executable(06exec "06/main.cpp")
add_executable(07exec "07/main.cpp")
add_executable(08exec "08/main.cpp")
add_executable(09exec "09/main.cpp")
add_executable(10exec "10/main.cpp")


add_executable(runall "mainrunner.cpp"
        "profile.h"
        "01/main.cpp"
        "02/main.cpp"
        "03/main.cpp"
        "04/main.cpp"
        "05/main.cpp"
        "06/main.cpp"
        "07/main.cpp"
        "08/main.cpp"
        "09/main.cpp"
        "10/main.cpp"

)
target_compile_definitions(runall PRIVATE RUNNER=1 )
