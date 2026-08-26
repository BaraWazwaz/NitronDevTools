cmake_minimum_required(VERSION 3.19.2)

# ==========================================
# 0. PROJECT CONFIGURATION NAMES
# ==========================================
set(PROJECT_NAME_VAR   "NitronDevTools")   # The root project name
set(LIB_TARGET_NAME    "nitron")           # Your library name
set(EXE_TARGET_NAME    "test")             # Leave empty "" to use the project name

# ==========================================
# 1. TOOLCHAIN & COMPILER CONFIGURATION
# ==========================================

# ONLY set compilers if this is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME_VAR)
    if(WIN32)
        set(CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe")
        set(CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe")
    endif()
endif()

project(${PROJECT_NAME_VAR} LANGUAGES CXX)

# Turn off the executable build by default if used as a subproject dependency
if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(BUILD_EXECUTABLE OFF CACHE BOOL "Build the main application executable" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default fallback configuration for single-config generators
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the build type: Debug or Release"
        FORCE
    )
endif()

# ==========================================
# 2. USER SWITCHES & OPTIONS
# ==========================================
option(BUILD_EXECUTABLE "Build the main application executable" ON)

# ==========================================
# 3. VENDORED DEPENDENCIES
# ==========================================
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# -------------------------
# fmt
# -------------------------
set(FMT_TEST OFF CACHE BOOL "Build fmt tests" FORCE)
set(FMT_DOC OFF CACHE BOOL "Build fmt documentation" FORCE)
set(FMT_INSTALL OFF CACHE BOOL "Generate fmt install target" FORCE)

add_subdirectory(${EXTERNAL_DIR}/fmt ${EXTERNAL_DIR}/fmt/build)

# ==========================================
# 4. INTERPROCEDURAL OPTIMIZATION (PROD ONLY)
# ==========================================
include(CheckIPOSupported)

check_ipo_supported(
    RESULT ipo_supported
    OUTPUT ipo_error
    LANGUAGES CXX
)

if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
else()
    message(STATUS "IPO/LTO is not supported: ${ipo_error}")
endif()

# ==========================================
# 5. INDEPENDENT LIBRARY TARGET (From ./lib/)
# ==========================================
file(GLOB LIB_SOURCES CONFIGURE_DEPENDS RECURSIVE "${CMAKE_CURRENT_SOURCE_DIR}/lib/**/*.cpp")

if(NOT LIB_SOURCES)
    message(FATAL_ERROR "No library source files found in ${CMAKE_CURRENT_SOURCE_DIR}/lib/")
endif()

add_library(${LIB_TARGET_NAME} ${LIB_SOURCES})

# Target Requirements
target_include_directories(${LIB_TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${LIB_TARGET_NAME} PUBLIC fmt::fmt)

target_compile_definitions(
    ${LIB_TARGET_NAME} 
    PUBLIC 
    RESOURCES_PATH=\"./resources/\"#"
)

# Profile Optimization Rules
target_compile_definitions(${LIB_TARGET_NAME}
    PUBLIC
        $<$<CONFIG:Debug>:DEVELOPMENT_MODE=1>
        $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(${LIB_TARGET_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:-Wall>
        $<$<CONFIG:Debug>:-Wextra>
        $<$<CONFIG:Debug>:-Wpedantic>

        $<$<CONFIG:Release>:-O3>
)

# Enable LTO solely for Release builds
if(ipo_supported)
    set_target_properties(${LIB_TARGET_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()

# ==========================================
# 6. CONDITIONAL EXECUTABLE TARGET (From ./src/)
# ==========================================
if(BUILD_EXECUTABLE)

    file(GLOB EXE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

    if(NOT EXE_SOURCES)
        message(FATAL_ERROR "No executable source files found in ${CMAKE_CURRENT_SOURCE_DIR}/src/")
    endif()

    add_executable(${EXE_TARGET_NAME} ${EXE_SOURCES})
    target_link_libraries(${EXE_TARGET_NAME} PRIVATE ${LIB_TARGET_NAME})

    # Static deployment options for portability
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_options(${EXE_TARGET_NAME} PRIVATE "-static-libgcc" "-static-libstdc++" "-static")
    endif()

endif()
