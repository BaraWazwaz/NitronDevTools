#!/usr/bin/env bash

set -euo pipefail

PROJECT_FILE="project.json"
BUILD_DIR="build"
DIST_DIR="dist"
SRC_DIR="src"
INCLUDE_DIR="include"

log() {
    printf '%s\n' "$*" >&2
}

error() {
    printf 'error: %s\n' "$*" >&2
    exit 1
}

require_command() {
    command -v "$1" >/dev/null 2>&1 || error "'$1' is required but was not found in PATH"
}

require_project_file() {
    [[ -f "$PROJECT_FILE" ]] || error "$PROJECT_FILE not found. Run './project.sh init' first."
}

require_python() {
    if command -v python3 >/dev/null 2>&1; then
        PYTHON="python3"
    elif command -v python >/dev/null 2>&1; then
        PYTHON="python"
    else
        error "Python 3 is required for project.json management."
    fi
}

project_value() {
    require_project_file
    require_python

    "$PYTHON" - "$PROJECT_FILE" "$1" <<'PY'
import json
import sys

filename = sys.argv[1]
key = sys.argv[2]

with open(filename, "r", encoding="utf-8") as f:
    data = json.load(f)

value = data.get(key, "")

if value is None:
    value = ""

print(value)
PY
}

project_name() {
    project_value "name"
}

project_version() {
    project_value "version"
}

project_executable() {
    local name
    name="$(project_name)"

    if [[ -n "${OS:-}" && "${OS}" == "Windows_NT" ]]; then
        printf '%s.exe\n' "$name"
    elif [[ "$(uname -s 2>/dev/null || true)" == MINGW* ||
            "$(uname -s 2>/dev/null || true)" == MSYS* ||
            "$(uname -s 2>/dev/null || true)" == CYGWIN* ]]; then
        printf '%s.exe\n' "$name"
    else
        printf '%s\n' "$name"
    fi
}

latest_git_tag() {
    local url="$1"

    require_command git

    log "🔎 Looking for the latest tag at $url..."

    local tags
    tags="$(
        git ls-remote --tags --refs "$url" 2>/dev/null |
        awk -F/ '{print $NF}' |
        grep -v '^$' |
        sort -V
    )" || true

    if [[ -z "$tags" ]]; then
        error "Could not find any Git tags at '$url'. Supply a tag explicitly."
    fi

    printf '%s\n' "$tags" | tail -n 1
}

version_from_tag() {
    local tag="$1"

    tag="${tag#v}"

    # Remove common non-semver prefixes/suffixes only when possible.
    if [[ "$tag" =~ ^[0-9]+(\.[0-9]+)* ]]; then
        printf '%s\n' "${BASH_REMATCH[0]}"
    else
        printf '%s\n' "$tag"
    fi
}

write_json_add_dependency() {
    local alias="$1"
    local url="$2"
    local version="$3"
    local tag="$4"
    local target="$5"
    local type="$6"

    require_python

    "$PYTHON" - "$PROJECT_FILE" "$alias" "$url" "$version" "$tag" "$target" "$type" <<'PY'
import json
import sys

filename = sys.argv[1]
alias = sys.argv[2]
url = sys.argv[3]
version = sys.argv[4]
tag = sys.argv[5]
target = sys.argv[6]
dep_type = sys.argv[7]

with open(filename, "r", encoding="utf-8") as f:
    data = json.load(f)

dependencies = data.setdefault("dependencies", [])

for dependency in dependencies:
    if dependency.get("alias") == alias:
        raise SystemExit(f"dependency '{alias}' already exists")

dependency = {
    "alias": alias,
    "url": url,
    "version": version,
    "tag": tag,
    "target": target,
    "type": dep_type
}

dependencies.append(dependency)

with open(filename, "w", encoding="utf-8") as f:
    json.dump(data, f, indent=4, ensure_ascii=False)
    f.write("\n")
PY
}

write_json_remove_dependency() {
    local alias="$1"

    require_python

    "$PYTHON" - "$PROJECT_FILE" "$alias" <<'PY'
import json
import sys

filename = sys.argv[1]
alias = sys.argv[2]

with open(filename, "r", encoding="utf-8") as f:
    data = json.load(f)

dependencies = data.get("dependencies", [])

new_dependencies = [
    dependency
    for dependency in dependencies
    if dependency.get("alias") != alias
]

if len(new_dependencies) == len(dependencies):
    raise SystemExit(f"dependency '{alias}' was not found")

data["dependencies"] = new_dependencies

with open(filename, "w", encoding="utf-8") as f:
    json.dump(data, f, indent=4, ensure_ascii=False)
    f.write("\n")
PY
}

init() {
    require_command cmake
    require_python

    log "⚙️  Initializing C++ project..."

    local default_name
    default_name="$(basename "$PWD")"

    mkdir -p "$SRC_DIR" "$INCLUDE_DIR" "$BUILD_DIR" "$DIST_DIR"

    if [[ ! -f "$PROJECT_FILE" ]]; then
        cat > "$PROJECT_FILE" <<EOF
{
    "schema": 1,
    "name": "$default_name",
    "url": "",
    "version": "0.1.0",
    "license": "MIT",
    "description": "",
    "language": "C++",
    "standard": 20,
    "dependencies": []
}
EOF
        log "📄 Created $PROJECT_FILE"
    else
        log "ℹ️  $PROJECT_FILE already exists"
    fi

    if [[ ! -f "CMakeLists.txt" ]]; then
        cat > "CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.19)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/project.json" PROJECT_JSON)

string(JSON PROJECT_NAME GET "${PROJECT_JSON}" "name")
string(JSON PROJECT_VERSION GET "${PROJECT_JSON}" "version")
string(JSON PROJECT_STANDARD GET "${PROJECT_JSON}" "standard")

if(PROJECT_NAME STREQUAL "PROJECT_NAME-NOTFOUND" OR PROJECT_NAME STREQUAL "")
    message(FATAL_ERROR "project.json does not contain a valid 'name'.")
endif()

if(PROJECT_VERSION STREQUAL "PROJECT_VERSION-NOTFOUND" OR PROJECT_VERSION STREQUAL "")
    set(PROJECT_VERSION "0.1.0")
endif()

if(PROJECT_STANDARD STREQUAL "PROJECT_STANDARD-NOTFOUND" OR PROJECT_STANDARD STREQUAL "")
    set(PROJECT_STANDARD 20)
endif()

project(${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD ${PROJECT_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

string(JSON DEPENDENCY_COUNT LENGTH "${PROJECT_JSON}" "dependencies")

if(DEPENDENCY_COUNT GREATER 0)
    math(EXPR LAST_DEPENDENCY_INDEX "${DEPENDENCY_COUNT} - 1")

    foreach(INDEX RANGE 0 ${LAST_DEPENDENCY_INDEX})
        string(JSON DEP_ALIAS GET "${PROJECT_JSON}" "dependencies" ${INDEX} "alias")
        string(JSON DEP_URL GET "${PROJECT_JSON}" "dependencies" ${INDEX} "url")
        string(JSON DEP_TAG GET "${PROJECT_JSON}" "dependencies" ${INDEX} "tag")
        string(JSON DEP_TARGET GET "${PROJECT_JSON}" "dependencies" ${INDEX} "target")

        if(DEP_ALIAS STREQUAL "" OR DEP_URL STREQUAL "" OR DEP_TAG STREQUAL "")
            message(FATAL_ERROR
                "Dependency at index ${INDEX} is missing alias, url, or tag."
            )
        endif()

        FetchContent_Declare(
            ${DEP_ALIAS}
            GIT_REPOSITORY "${DEP_URL}"
            GIT_TAG "${DEP_TAG}"
        )
    endforeach()

    foreach(INDEX RANGE 0 ${LAST_DEPENDENCY_INDEX})
        string(JSON DEP_ALIAS GET "${PROJECT_JSON}" "dependencies" ${INDEX} "alias")
        FetchContent_MakeAvailable(${DEP_ALIAS})
    endforeach()
endif()

add_subdirectory(src)
EOF
        log "📄 Created CMakeLists.txt"
    else
        log "ℹ️  CMakeLists.txt already exists"
    fi

    if [[ ! -f "$SRC_DIR/CMakeLists.txt" ]]; then
        cat > "$SRC_DIR/CMakeLists.txt" <<'EOF'
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
EOF
        log "📄 Created src/CMakeLists.txt"
    else
        log "ℹ️  src/CMakeLists.txt already exists"
    fi

    log ""
    log "✅ Project initialized."
    log ""
    log "Next steps:"
    log "  ./project.sh add fmt https://github.com/fmtlib/fmt 11.0.2 --target fmt::fmt"
    log "  ./project.sh compile"
    log "  ./project.sh run"
}

add_dependency() {
    require_project_file
    require_command git

    if [[ $# -lt 2 ]]; then
        error "usage: ./project.sh add <alias> <url> [tag] [options]"
    fi

    local alias="$1"
    local url="$2"
    shift 2

    local tag=""
    local target=""
    local version=""
    local dep_type="git"

    # Optional positional tag.
    if [[ $# -gt 0 && "${1:0:2}" != "--" ]]; then
        tag="$1"
        shift
    fi

    while [[ $# -gt 0 ]]; do
        case "$1" in
            --tag)
                [[ $# -ge 2 ]] || error "--tag requires a value"
                tag="$2"
                shift 2
                ;;

            --version)
                [[ $# -ge 2 ]] || error "--version requires a value"
                version="$2"
                shift 2
                ;;

            --target)
                [[ $# -ge 2 ]] || error "--target requires a value"
                target="$2"
                shift 2
                ;;

            --type)
                [[ $# -ge 2 ]] || error "--type requires a value"
                dep_type="$2"
                shift 2
                ;;

            *)
                error "unknown option '$1'"
                ;;
        esac
    done

    if [[ "$dep_type" != "git" ]]; then
        error "only dependency type 'git' is currently supported"
    fi

    if [[ -z "$tag" ]]; then
        tag="$(latest_git_tag "$url")"
        log "📌 Using latest tag: $tag"
    fi

    if [[ -z "$version" ]]; then
        version="$(version_from_tag "$tag")"
    fi

    if [[ -z "$target" ]]; then
        error "A CMake target is required. Use --target <target>, for example --target fmt::fmt."
    fi

    log "➕ Adding dependency '$alias'..."
    log "   URL:     $url"
    log "   Version: $version"
    log "   Tag:     $tag"
    log "   Target:  $target"

    write_json_add_dependency \
        "$alias" \
        "$url" \
        "$version" \
        "$tag" \
        "$target" \
        "$dep_type"

    # Recreate the CMake build from project.json.
    rm -rf "$BUILD_DIR"

    mkdir -p "$BUILD_DIR"

    log "✅ Dependency '$alias' added."
    log "🧹 Build directory reset so CMake will fetch the new dependency."
}

remove_dependency() {
    require_project_file

    [[ $# -eq 1 ]] || error "usage: ./project.sh remove <alias>"

    local alias="$1"

    log "➖ Removing dependency '$alias'..."

    write_json_remove_dependency "$alias"

    # Removing the entire build tree is intentional.
    #
    # FetchContent stores dependency state inside the build tree.
    # Recreating it guarantees the removed dependency is no longer
    # present in the CMake build.
    rm -rf "$BUILD_DIR"

    log "🧹 Build directory removed."
    log "✅ Dependency '$alias' removed from project.json and build state."
}

configure() {
    require_project_file
    require_command cmake

    mkdir -p "$BUILD_DIR"

    cmake -S . -B "$BUILD_DIR" -G "MinGW Makefiles"
}

compile() {
    require_project_file
    require_command cmake

    log "🔨 Configuring project..."

    configure

    log "🚀 Building project..."

    cmake --build "$BUILD_DIR" --parallel

    local executable
    executable="$(project_executable)"

    if [[ -f "$DIST_DIR/$executable" ]]; then
        log ""
        log "✅ Build successful."
        log "📦 Executable: $DIST_DIR/$executable"
    else
        log ""
        log "⚠️  Build completed, but expected executable was not found:"
        log "   $DIST_DIR/$executable"
    fi
}

run_project() {
    require_project_file

    local executable
    executable="$(project_executable)"

    local executable_path="$DIST_DIR/$executable"

    if [[ ! -f "$executable_path" ]]; then
        log "⚠️  Executable not found. Building first..."
        compile
    fi

    [[ -f "$executable_path" ]] ||
        error "Executable '$executable_path' does not exist."

    log "🚀 Running $executable..."

    if [[ "$executable_path" == *.exe ]]; then
        "$executable_path" "$@"
    else
        "./$executable_path" "$@"
    fi
}

clean() {
    log "🧹 Cleaning generated build artifacts..."

    rm -rf "$BUILD_DIR" "$DIST_DIR"

    log "✅ Removed build/ and dist/."
}

show_help() {
    cat >&2 <<'EOF'
Usage:
    ./project.sh <command>

Commands:
    init
        Initialize the project structure and create missing project files.

    add <alias> <url> [tag] [options]
        Add a Git dependency to project.json.

        Options:
            --tag <tag>
            --version <version>
            --target <cmake-target>
            --type <type>

        If tag is omitted, the latest Git tag is selected.

        Example:
            ./project.sh add fmt \
                https://github.com/fmtlib/fmt \
                11.0.2 \
                --target fmt::fmt

    remove <alias>
        Remove a dependency from project.json and reset build state.

    compile
        Configure and compile the project.

    run [args...]
        Run the compiled executable.

    clean
        Remove build/ and dist/.

    help
        Show this help message.
EOF
}

main() {
    local command="${1:-help}"

    case "$command" in
        init)
            shift
            init "$@"
            ;;

        add)
            shift
            add_dependency "$@"
            ;;

        remove)
            shift
            remove_dependency "$@"
            ;;

        compile)
            shift
            compile "$@"
            ;;

        run)
            shift
            run_project "$@"
            ;;

        clean)
            shift
            clean "$@"
            ;;

        help|-h|--help)
            show_help
            ;;

        *)
            error "unknown command '$command'. Run './project.sh help'."
            ;;
    esac
}

main "$@"
