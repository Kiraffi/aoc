cmake_minimum_required(VERSION 3.19)
project(aoc2024)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_UNITY_BUILD ON)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Adding SDL
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
        SDL
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG acf0f09
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

find_package(Vulkan)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    get_filename_component(DIR ${SHADER} DIRECTORY)

    string(REPLACE "." "_" FILENAME_UNDERSCORE "${FILENAME}")
    message("Shader: " ${FILENAME_UNDERSCORE} " array: " ${FILENAME_UNDERSCORE})

    set(SHADER_OUT_FILE ${DIR}/${FILENAME_UNDERSCORE}.h)
    add_custom_command(OUTPUT ${SHADER_OUT_FILE}
        COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${SHADER} -o ${SHADER_OUT_FILE} --vn ${FILENAME_UNDERSCORE}
        DEPENDS ${SHADER}
        COMMENT "Compiling $DIR"/"${FILENAME}")

    list(APPEND HEADER_SHADERS ${SHADER_OUT_FILE})
endForeach()



add_custom_target(shadertarget ALL DEPENDS ${HEADER_SHADERS})



#print_variables()

include_directories("${sdl_SOURCE_DIR}/src")
include_directories("src")

#if (MSVC)
#    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
#endif (MSVC)

function(make_prog exe prog)

    add_executable(${exe}
        ${SHADERS}
        ${prog}
        src/commons.cpp
        src/commonrender.cpp
    )

    add_dependencies(${exe} shadertarget)

    target_link_libraries(${exe} PUBLIC
            SDL3::SDL3
            #glm::glm
    )

endfunction(make_prog)

make_prog("01" "src/01/d01.cpp")
make_prog("02" "src/02/d02.cpp")
make_prog("03" "src/03/d03.cpp")
make_prog("04" "src/04/d04.cpp")
make_prog("05" "src/05/d05.cpp")
make_prog("06" "src/06/d06.cpp")
