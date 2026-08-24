# ---------------------------------------------------------------------------
# Collect library sources
# ---------------------------------------------------------------------------

file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS

    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

# main.cpp belongs to the executable, not the library.
set(PROJECT_MAIN_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

if(EXISTS "${PROJECT_MAIN_SOURCE}")
    list(REMOVE_ITEM
        PROJECT_SOURCES
        "${PROJECT_MAIN_SOURCE}"
    )
endif()

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------

if(NOT PROJECT_SOURCES)
    message(FATAL_ERROR
        "No library C++ source files were found in "
        "${CMAKE_CURRENT_SOURCE_DIR}."
    )
endif()

# The actual library target.
#
# Example:
#     project.json -> "name": "nitron"
#
# creates:
#
#     nitron
#
add_library(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

# Public namespaced target.
#
# Consumers should use:
#
#     nitron::nitron
#
add_library(${PROJECT_NAME}::${PROJECT_NAME}
    ALIAS
    ${PROJECT_NAME}
)

# ---------------------------------------------------------------------------
# Library include directories
# ---------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${PROJECT_INCLUDE_DIR}"
)

# ---------------------------------------------------------------------------
# Library properties
# ---------------------------------------------------------------------------

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# ---------------------------------------------------------------------------
# Library dependencies
# ---------------------------------------------------------------------------

string(JSON DEPENDENCY_COUNT
    LENGTH
    "${PROJECT_JSON}"
    "dependencies"
)

if(DEPENDENCY_COUNT GREATER 0)

    math(EXPR LAST_DEPENDENCY_INDEX
        "${DEPENDENCY_COUNT} - 1"
    )

    foreach(INDEX RANGE 0 ${LAST_DEPENDENCY_INDEX})

        string(JSON DEP_ALIAS
            GET "${PROJECT_JSON}"
            "dependencies" ${INDEX} "alias"
        )

        string(JSON DEP_TARGET
            GET "${PROJECT_JSON}"
            "dependencies" ${INDEX} "target"
        )

        if(DEP_TARGET STREQUAL "" OR
           DEP_TARGET STREQUAL "DEP_TARGET-NOTFOUND")

            message(FATAL_ERROR
                "Dependency '${DEP_ALIAS}' does not specify a "
                "valid CMake 'target'."
            )
        endif()

        if(NOT TARGET ${DEP_TARGET})
            message(FATAL_ERROR
                "Dependency '${DEP_ALIAS}' requested target "
                "'${DEP_TARGET}', but that CMake target does not exist."
            )
        endif()

        target_link_libraries(${PROJECT_NAME}
            PUBLIC
                ${DEP_TARGET}
        )

    endforeach()

endif()

# ---------------------------------------------------------------------------
# Optional executable
# ---------------------------------------------------------------------------

if(EXISTS "${PROJECT_MAIN_SOURCE}")

    set(PROJECT_EXECUTABLE
        "${PROJECT_NAME}_app"
    )

    add_executable(${PROJECT_EXECUTABLE}
        "${PROJECT_MAIN_SOURCE}"
    )

    target_link_libraries(${PROJECT_EXECUTABLE}
        PRIVATE
            ${PROJECT_NAME}
    )

    target_include_directories(${PROJECT_EXECUTABLE}
        PRIVATE
            "${PROJECT_INCLUDE_DIR}"
    )

    set_target_properties(${PROJECT_EXECUTABLE} PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist"
    )

endif()
