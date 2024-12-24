cmake_minimum_required(VERSION 3.19)
project(aoc2024)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_UNITY_BUILD ON)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Adding SDL
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
        SDL
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG 3344270
)

FetchContent_MakeAvailable(SDL)

function(PRINT_VARIABLES)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()



set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE SHADERS ${SHADER_DIR}/*.vert ${SHADER_DIR}/*.frag ${SHADER_DIR}/*.comp)

#remove the included shaders.
list(REMOVE_ITEM SHADERS ${SHADER_DIR}/int64.comp)

find_package(Vulkan)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    get_filename_component(DIR ${SHADER} DIRECTORY)
    string(REPLACE "." "_" FILENAME_UNDERSCORE "${FILENAME}")
    message("Found shader: ${DIR}/${FILENAME_UNDERSCORE}")

    set(SHADER_OUT_FILE ${DIR}/${FILENAME_UNDERSCORE}.h)
    add_custom_command(OUTPUT ${SHADER_OUT_FILE}
        COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${SHADER} -o ${SHADER_OUT_FILE} --vn ${FILENAME_UNDERSCORE}
        DEPENDS ${SHADER}
        COMMENT "Compiling $DIR / ${FILENAME}")

    list(APPEND HEADER_SHADERS ${SHADER_OUT_FILE})
endForeach()



add_custom_target(shadertarget ALL DEPENDS ${HEADER_SHADERS})



#print_variables()

include_directories("${sdl_SOURCE_DIR}/src")
include_directories("src")

#if (MSVC)
#    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
#endif (MSVC)

add_library(sharedlibs
    ${SHADERS}
    src/commons.cpp
    src/commonrender.cpp
)

target_link_libraries(sharedlibs PUBLIC
    SDL3::SDL3
    #glm::glm
)

function(make_prog exe prog)

    add_executable(${exe}
        #${SHADERS}
        ${prog}
        #src/commons.cpp
        #src/commonrender.cpp
    )

    add_dependencies(${exe} shadertarget)

    target_link_libraries(${exe} PUBLIC
        sharedlibs
    )

endfunction(make_prog)

make_prog("01" "src/01/d01.cpp")
make_prog("02" "src/02/d02.cpp")
make_prog("03" "src/03/d03.cpp")
make_prog("04" "src/04/d04.cpp")
make_prog("05" "src/05/d05.cpp")
make_prog("06" "src/06/d06.cpp")
make_prog("07" "src/07/d07.cpp")
make_prog("08" "src/08/d08.cpp")
make_prog("09" "src/09/d09.cpp")
make_prog("10" "src/10/d10.cpp")
make_prog("11" "src/11/d11.cpp")
make_prog("12" "src/12/d12.cpp")
make_prog("13" "src/13/d13.cpp")
make_prog("14" "src/14/d14.cpp")
make_prog("15" "src/15/d15.cpp")
make_prog("16" "src/16/d16.cpp")
make_prog("17" "src/17/d17.cpp")
make_prog("18" "src/18/d18.cpp")
make_prog("19" "src/19/d19.cpp")
make_prog("20" "src/20/d20.cpp")
make_prog("21" "src/21/d21.cpp")
make_prog("22" "src/22/d22.cpp")
make_prog("23" "src/23/d23.cpp")
#make_prog("24" "src/24/d24.cpp")
#make_prog("25" "src/25/d25.cpp")

