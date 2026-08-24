#!/usr/bin/env bash

set -e

log() {
    echo "$@" >&2
}

prepare() {
    log "⚙️  Preparing CMake generation and build process..."
    
    if [ ! -d "build" ]; then
        mkdir build
        log "📁 Created 'build' directory."
    fi
    
    cd build
    
    log "🧹 Cleaning previous build cache..."
    rm -rf CMakeCache.txt CMakeFiles/
    
    log "🔨 Generating build system..."
    cmake -G "MinGW Makefiles" ..

    log "✅ Success! CMake project was prepared"
    cd ..
}

compile() {
    log "🚀 Compiling source code..."
    
    if cmake --build build; then
        log "✅ Success! Output created in dist/program.exe"
        return 0
    else
        log "❌ Error: Failed to build"
        return 1
    fi
}

run() {
    shift
    log "🚀 Running program..."
    
    if [[ ! -f "dist/program.exe" ]]; then
        log "❌ dist/program.exe was not found..."
        
        set +e
        compile
        local compile_status=$?
        set -e

        if [ $compile_status -ne 0 ]; then
            return $compile_status
        fi
    fi

    set +e
    ./dist/program.exe "$@"
    local run_status=$?
    set -e
    
    if [ $run_status -eq 0 ]; then
        log "✅ Exited with status code 0"
    else
        log "❌ Exited with status code $run_status"
    fi

    return $run_status
}

clean() {
    log "🧹 Clean-targeting local project object files..."
    
    if [ -d "dist" ]; then
        rm -rf dist
        log "🗑️  Removed 'dist/' output directory."
    fi

    if [ -d "build/src" ]; then
        rm -rf "build/src"
        log "🗑️  Purged local object file tracking tree ('build/src/')."
        log "✅ Success! Application artifacts wiped. Dependencies fully preserved."
    else
        log "ℹ️  No local object files found to clean."
    fi

    return 0
}

show_help() {
    log "❌ Error: Invalid or missing argument."
    log "💡 Usage: ./script.sh [command]"
    log "Available commands:"
    log "  prepare     - Generate CMake build files and install packages"
    log "  compile     - Compile the program"
    log "  run <args>  - Run the program \`./script.sh run <args>\`"
    log "  clean       - Remove project's build artifacts"
}

main() {
    local command="$1"

    case "$command" in
        "prepare")
            prepare
            return 0
            ;;
        "compile")
            compile
            return 0
            ;;
        "run")
            run "$@"
            return 0
            ;;
        "clean")
            clean
            return 0
            ;;
        *)
            show_help
            exit 1
            ;;
    esac
}

main "$@"
