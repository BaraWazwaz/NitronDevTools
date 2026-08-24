cmake_minimum_required(VERSION 3.19)
project(MyProgram CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist")

include(FetchContent)

set(DYNAMIC_LINK_TARGETS "" CACHE INTERNAL "Global dynamic links")

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/packages.json")
    message(FATAL_ERROR "packages.json file not found in root directory!")
endif()

file(READ "${CMAKE_SOURCE_DIR}/packages.json" JSON_DATA)

string(JSON DEPS_LENGTH LENGTH "${JSON_DATA}" "dependencies")
math(EXPR LOOP_MAX "${DEPS_LENGTH} - 1")

foreach(INDEX RANGE ${LOOP_MAX})
    string(JSON DEP_NAME   GET "${JSON_DATA}" "dependencies" ${INDEX} "name")
    string(JSON DEP_URL    GET "${JSON_DATA}" "dependencies" ${INDEX} "url")
    string(JSON DEP_TAG    GET "${JSON_DATA}" "dependencies" ${INDEX} "tag")
    string(JSON DEP_TARGET GET "${JSON_DATA}" "dependencies" ${INDEX} "target")

    FetchContent_Declare(
        ${DEP_NAME}
        GIT_REPOSITORY "${DEP_URL}"
        GIT_TAG        "${DEP_TAG}"
    )
    FetchContent_MakeAvailable(${DEP_NAME})

    list(APPEND DYNAMIC_LINK_TARGETS ${DEP_TARGET})
endforeach()
set(DYNAMIC_LINK_TARGETS "${DYNAMIC_LINK_TARGETS}" CACHE INTERNAL "Global dynamic links")

add_subdirectory(src)
