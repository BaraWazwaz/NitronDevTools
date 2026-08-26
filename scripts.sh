#!/bin/env bash

# 1. CMake Set up
step_001() {
    nano ./CMakeLists.txt
    # write its content
}

# 2. Build Set up
step_002() {
    mkdir ./build
    cd ./build
    cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug   .. # Development
    # cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .. # Production
    cd ..
}

# 3. Build Projects' Executables
step_003() {
    cmake --build ./build
}

# 4. Run Executables
step_004() {
    ./MyProject.exe
}

# 5. Add Resources to be recognized
step_005() {
    echo "Hi" > ../resources/file.txt
    # Note: 
    #     You have to be at root when you call program for it to 
    #     be rendered correctly in case of relative resources path
}

echo "Open this file to view the scripts, this script file does not run anything by itself"
