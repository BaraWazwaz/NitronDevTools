# ============================================================================
# Discover library sources
# ============================================================================

file(GLOB_RECURSE PROJECT_LIBRARY_SOURCES
    CONFIGURE_DEPENDS

    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

# main.cpp belongs to the executable, not the library.
list(FILTER PROJECT_LIBRARY_SOURCES
    EXCLUDE REGEX
    ".*/main\\.cpp$"
)

# ============================================================================
# Create library
# ============================================================================

if(PROJECT_LIBRARY_SOURCES)

    # ------------------------------------------------------------------------
    # Compiled library
    # ------------------------------------------------------------------------

    add_library(${PROJECT_NAME}
        STATIC
            ${PROJECT_LIBRARY_SOURCES}
    )

    # Public namespaced target.
    add_library(${PROJECT_NAME}::${PROJECT_NAME}
        ALIAS
            ${PROJECT_NAME}
    )

    target_include_directories(${PROJECT_NAME}
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )

    target_compile_features(${PROJECT_NAME}
        PUBLIC
            cxx_std_${PROJECT_STANDARD}
    )

    set(PROJECT_DEPENDENCY_SCOPE PUBLIC)

else()

    # ------------------------------------------------------------------------
    # Header-only library
    # ------------------------------------------------------------------------

    add_library(${PROJECT_NAME}
        INTERFACE
    )

    # Public namespaced target.
    add_library(${PROJECT_NAME}::${PROJECT_NAME}
        ALIAS
            ${PROJECT_NAME}
    )

    target_include_directories(${PROJECT_NAME}
        INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )

    target_compile_features(${PROJECT_NAME}
        INTERFACE
            cxx_std_${PROJECT_STANDARD}
    )

    set(PROJECT_DEPENDENCY_SCOPE INTERFACE)

endif()

# ============================================================================
# Link dependencies into the library
# ============================================================================

string(JSON DEPENDENCY_COUNT
    LENGTH "${PROJECT_JSON}" "dependencies"
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

        if(DEP_TARGET STREQUAL "")
            message(FATAL_ERROR
                "Dependency '${DEP_ALIAS}' does not specify "
                "a CMake 'target'."
            )
        endif()

        if(NOT TARGET ${DEP_TARGET})
            message(FATAL_ERROR
                "Dependency '${DEP_ALIAS}' requested target "
                "'${DEP_TARGET}', but that CMake target does not exist."
            )
        endif()

        target_link_libraries(${PROJECT_NAME}
            ${PROJECT_DEPENDENCY_SCOPE}
                ${DEP_TARGET}
        )

    endforeach()

endif()

# ============================================================================
# Optional executable
# ============================================================================

set(PROJECT_MAIN
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

if(EXISTS "${PROJECT_MAIN}")

    add_executable(${PROJECT_NAME}_app
        "${PROJECT_MAIN}"
    )

    target_link_libraries(${PROJECT_NAME}_app
        PRIVATE
            ${PROJECT_NAME}::${PROJECT_NAME}
    )

    target_include_directories(${PROJECT_NAME}_app
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )

    target_compile_features(${PROJECT_NAME}_app
        PRIVATE
            cxx_std_${PROJECT_STANDARD}
    )

    set_target_properties(${PROJECT_NAME}_app PROPERTIES
        OUTPUT_NAME
            "${PROJECT_NAME}"

        RUNTIME_OUTPUT_DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/../dist"
    )

endif()
