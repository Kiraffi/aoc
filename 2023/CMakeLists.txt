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
add_executable(11exec "11/main.cpp")
add_executable(12exec "12/main.cpp")
add_executable(13exec "13/main.cpp")
add_executable(14exec "14/main.cpp")
add_executable(15exec "15/main.cpp")
add_executable(16exec "16/main.cpp")
add_executable(17exec "17/main.cpp")
add_executable(18exec "18/main.cpp")
add_executable(19exec "19/main.cpp")
add_executable(20exec "20/main.cpp")
add_executable(21exec "21/main.cpp")
add_executable(22exec "22/main.cpp")
add_executable(23exec "23/main.cpp")
add_executable(24exec "24/main.cpp")
add_executable(25exec "25/main.cpp")


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
        "11/main.cpp"
        "12/main.cpp"
        "13/main.cpp"
        "14/main.cpp"
        "15/main.cpp"
        "16/main.cpp"
        "17/main.cpp"
        "18/main.cpp"
        "19/main.cpp"
        "20/main.cpp"
        "21/main.cpp"
        "22/main.cpp"
        "23/main.cpp"
        "24/main.cpp"
        "25/main.cpp"
)
target_compile_definitions(runall PRIVATE RUNNER=1 )
