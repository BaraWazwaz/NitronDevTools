file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

if(NOT PROJECT_SOURCES)
    message(FATAL_ERROR
        "No C++ source files were found in ${CMAKE_CURRENT_SOURCE_DIR}."
    )
endif()

string(MAKE_C_IDENTIFIER
    "${PROJECT_NAME}_${PROJECT_VERSION}"
    PROJECT_TARGET_BASE
)

string(SHA1 PROJECT_SOURCE_HASH "${CMAKE_SOURCE_DIR}")

string(SUBSTRING
    "${PROJECT_SOURCE_HASH}"
    0
    10
    PROJECT_SOURCE_HASH_SHORT
)

set(PROJECT_TARGET
    "${PROJECT_TARGET_BASE}_${PROJECT_SOURCE_HASH_SHORT}_app"
)

add_executable(${PROJECT_TARGET}
    ${PROJECT_SOURCES}
)

target_include_directories(${PROJECT_TARGET}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
)

set_target_properties(${PROJECT_TARGET} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist"
)

string(JSON DEPENDENCY_COUNT LENGTH "${PROJECT_JSON}" "dependencies")

if(DEPENDENCY_COUNT GREATER 0)
    math(EXPR LAST_DEPENDENCY_INDEX "${DEPENDENCY_COUNT} - 1")

    foreach(INDEX RANGE 0 ${LAST_DEPENDENCY_INDEX})
        string(JSON DEP_ALIAS GET "${PROJECT_JSON}" "dependencies" ${INDEX} "alias")
        string(JSON DEP_TARGET GET "${PROJECT_JSON}" "dependencies" ${INDEX} "target")

        if(DEP_TARGET STREQUAL "")
            message(FATAL_ERROR
                "Dependency '${DEP_ALIAS}' does not specify a CMake 'target'."
            )
        endif()

        if(NOT TARGET ${DEP_TARGET})
            message(FATAL_ERROR
                "Dependency '${DEP_ALIAS}' requested target '${DEP_TARGET}', "
                "but that CMake target does not exist."
            )
        endif()

        target_link_libraries(${PROJECT_TARGET}
            PRIVATE
                ${DEP_TARGET}
        )
    endforeach()
endif()
