cmake_minimum_required(VERSION 3.19)

# ============================================================================
# Read project.json
# ============================================================================

file(READ
    "${CMAKE_CURRENT_SOURCE_DIR}/project.json"
    PROJECT_JSON
)

string(JSON PROJECT_NAME
    GET "${PROJECT_JSON}" "name"
)

string(JSON PROJECT_VERSION
    GET "${PROJECT_JSON}" "version"
)

string(JSON PROJECT_STANDARD
    GET "${PROJECT_JSON}" "standard"
)

if(PROJECT_NAME STREQUAL "PROJECT_NAME-NOTFOUND" OR
   PROJECT_NAME STREQUAL "")

    message(FATAL_ERROR
        "project.json does not contain a valid 'name'."
    )

endif()

if(PROJECT_VERSION STREQUAL "PROJECT_VERSION-NOTFOUND" OR
   PROJECT_VERSION STREQUAL "")

    set(PROJECT_VERSION "0.1.0")

endif()

if(PROJECT_STANDARD STREQUAL "PROJECT_STANDARD-NOTFOUND" OR
   PROJECT_STANDARD STREQUAL "")

    set(PROJECT_STANDARD 20)

endif()

# ============================================================================
# Project
# ============================================================================

project(${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    LANGUAGES CXX
)

# ============================================================================
# C++ configuration
# ============================================================================

set(CMAKE_CXX_STANDARD
    ${PROJECT_STANDARD}
)

set(CMAKE_CXX_STANDARD_REQUIRED
    ON
)

set(CMAKE_CXX_EXTENSIONS
    OFF
)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
)

# ============================================================================
# Project directories
# ============================================================================

set(PROJECT_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set(PROJECT_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ============================================================================
# Dependencies
# ============================================================================

include(FetchContent)

string(JSON DEPENDENCY_COUNT
    LENGTH
    "${PROJECT_JSON}"
    "dependencies"
)

if(DEPENDENCY_COUNT GREATER 0)

    math(EXPR LAST_DEPENDENCY_INDEX
        "${DEPENDENCY_COUNT} - 1"
    )

    # ------------------------------------------------------------------------
    # Declare dependencies
    # ------------------------------------------------------------------------

    foreach(INDEX RANGE 0 ${LAST_DEPENDENCY_INDEX})

        string(JSON DEP_ALIAS
            GET
            "${PROJECT_JSON}"
            "dependencies"
            ${INDEX}
            "alias"
        )

        string(JSON DEP_URL
            GET
            "${PROJECT_JSON}"
            "dependencies"
            ${INDEX}
            "url"
        )

        string(JSON DEP_TAG
            GET
            "${PROJECT_JSON}"
            "dependencies"
            ${INDEX}
            "tag"
        )

        string(JSON DEP_TARGET
            GET
            "${PROJECT_JSON}"
            "dependencies"
            ${INDEX}
            "target"
        )

        if(DEP_ALIAS STREQUAL "" OR
           DEP_ALIAS STREQUAL "DEP_ALIAS-NOTFOUND")

            message(FATAL_ERROR
                "Dependency at index ${INDEX} is missing 'alias'."
            )

        endif()

        if(DEP_URL STREQUAL "" OR
           DEP_URL STREQUAL "DEP_URL-NOTFOUND")

            message(FATAL_ERROR
                "Dependency '${DEP_ALIAS}' is missing 'url'."
            )

        endif()

        if(DEP_TAG STREQUAL "" OR
           DEP_TAG STREQUAL "DEP_TAG-NOTFOUND")

            message(FATAL_ERROR
                "Dependency '${DEP_ALIAS}' is missing 'tag'."
            )

        endif()

        if(DEP_TARGET STREQUAL "" OR
           DEP_TARGET STREQUAL "DEP_TARGET-NOTFOUND")

            message(FATAL_ERROR
                "Dependency '${DEP_ALIAS}' is missing 'target'."
            )

        endif()

        FetchContent_Declare(
            ${DEP_ALIAS}

            GIT_REPOSITORY
                "${DEP_URL}"

            GIT_TAG
                "${DEP_TAG}"
        )

    endforeach()

    # ------------------------------------------------------------------------
    # Make dependencies available
    # ------------------------------------------------------------------------

    foreach(INDEX RANGE 0 ${LAST_DEPENDENCY_INDEX})

        string(JSON DEP_ALIAS
            GET
            "${PROJECT_JSON}"
            "dependencies"
            ${INDEX}
            "alias"
        )

        FetchContent_MakeAvailable(
            ${DEP_ALIAS}
        )

    endforeach()

endif()

# ============================================================================
# Project targets
# ============================================================================

add_subdirectory(
    "${PROJECT_SRC_DIR}"
)
