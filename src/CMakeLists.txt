# ============================================================================
# Find all C++ source files
# ============================================================================

file(GLOB_RECURSE PROJECT_ALL_SOURCES
    CONFIGURE_DEPENDS

    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

# ============================================================================
# Find main.cpp
#
# main.cpp belongs to the executable, not the reusable library.
# ============================================================================

set(PROJECT_MAIN_SOURCE "")

foreach(SOURCE_FILE IN LISTS PROJECT_ALL_SOURCES)

    get_filename_component(
        SOURCE_NAME
        "${SOURCE_FILE}"
        NAME
    )

    if(SOURCE_NAME STREQUAL "main.cpp")

        if(PROJECT_MAIN_SOURCE)

            message(FATAL_ERROR
                "Multiple main.cpp files were found:\n"
                "  ${PROJECT_MAIN_SOURCE}\n"
                "  ${SOURCE_FILE}\n"
                "Only one executable entry point is allowed."
            )

        endif()

        set(PROJECT_MAIN_SOURCE
            "${SOURCE_FILE}"
        )

    endif()

endforeach()

# ============================================================================
# Library sources
#
# Everything except main.cpp belongs to the reusable library.
# ============================================================================

set(PROJECT_LIBRARY_SOURCES
    ${PROJECT_ALL_SOURCES}
)

if(PROJECT_MAIN_SOURCE)

    list(REMOVE_ITEM
        PROJECT_LIBRARY_SOURCES
        "${PROJECT_MAIN_SOURCE}"
    )

endif()

# ============================================================================
# Library target
# ============================================================================

if(PROJECT_LIBRARY_SOURCES)

    # ------------------------------------------------------------------------
    # Normal library project
    #
    # The project contains .cpp/.cc/.cxx implementation files.
    # ------------------------------------------------------------------------

    add_library(
        ${PROJECT_NAME}
        STATIC
            ${PROJECT_LIBRARY_SOURCES}
    )

else()

    # ------------------------------------------------------------------------
    # Header-only / executable-only project
    #
    # There are no library implementation files.
    #
    # INTERFACE allows us to expose a stable target anyway.
    # ------------------------------------------------------------------------

    add_library(
        ${PROJECT_NAME}
        INTERFACE
    )

endif()

# ============================================================================
# Public target
#
# This is the target consumers should use.
#
# Example:
#
#     nitron::nitron
#
#     yugioh::yugioh
#
# ============================================================================

add_library(
    ${PROJECT_NAME}::${PROJECT_NAME}
    ALIAS
    ${PROJECT_NAME}
)

# ============================================================================
# Public include directory
# ============================================================================

if(EXISTS "${PROJECT_INCLUDE_DIR}")

    target_include_directories(
        ${PROJECT_NAME}

        INTERFACE
            "${PROJECT_INCLUDE_DIR}"
    )

endif()

# If this is a compiled library, its own source compilation also needs
# access to the public headers.
if(PROJECT_LIBRARY_SOURCES)

    target_include_directories(
        ${PROJECT_NAME}

        PUBLIC
            "${PROJECT_INCLUDE_DIR}"
    )

endif()

# ============================================================================
# Library dependencies
# ============================================================================

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
            GET
            "${PROJECT_JSON}"
            "dependencies"
            ${INDEX}
            "alias"
        )

        string(JSON DEP_TARGET
            GET
            "${PROJECT_JSON}"
            "dependencies"
            ${INDEX}
            "target"
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

        target_link_libraries(
            ${PROJECT_NAME}

            INTERFACE
                ${DEP_TARGET}
        )

    endforeach()

endif()

# ============================================================================
# Optional executable
#
# If main.cpp exists, build an executable that links against the library.
# ============================================================================

if(PROJECT_MAIN_SOURCE)

    set(PROJECT_EXECUTABLE
        "${PROJECT_NAME}_app"
    )

    add_executable(
        ${PROJECT_EXECUTABLE}
        "${PROJECT_MAIN_SOURCE}"
    )

    target_link_libraries(
        ${PROJECT_EXECUTABLE}
        PRIVATE
            ${PROJECT_NAME}
    )

    if(EXISTS "${PROJECT_INCLUDE_DIR}")

        target_include_directories(
            ${PROJECT_EXECUTABLE}
            PRIVATE
                "${PROJECT_INCLUDE_DIR}"
        )

    endif()

    set_target_properties(
        ${PROJECT_EXECUTABLE}
        PROPERTIES
            OUTPUT_NAME
                "${PROJECT_NAME}"
            RUNTIME_OUTPUT_DIRECTORY
                "${CMAKE_SOURCE_DIR}/dist"
    )

endif()
