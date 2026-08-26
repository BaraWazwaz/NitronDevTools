cmake_minimum_required(VERSION 3.19.2)

# ==========================================
# 0. PROJECT CONFIGURATION NAMES
# ==========================================
set(PROJECT_NAME_VAR   "NitronDevTools")   # Standalone root name
set(LIB_TARGET_NAME    "nitron")           # Library name
set(EXE_TARGET_NAME    "nitron_test")      # Executable name

# ==========================================
# 1. TOOLCHAIN & DEPENDENCY CONFIGURATION
# ==========================================
# Note: We cannot check CMAKE_PROJECT_NAME here because project() isn't declared yet.
# We explicitly set compilers only if this CMakeLists.txt is the root entry point.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(IS_SUBPROJECT FALSE)
    if(WIN32)
        set(CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe")
        set(CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe")
    endif()
else()
    set(IS_SUBPROJECT TRUE)
endif()

project(${PROJECT_NAME_VAR} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Handle Build Type / Production Mode forcing
if(IS_SUBPROJECT)
    # FORCE Production/Release optimizations for nitron targets even if parent is Debug
    message(STATUS "[nitron] Building dependency targets in Production (Release) mode.")
else()
    # Standalone default fallback
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type" FORCE)
    endif()
endif()

# ==========================================
# 2. USER SWITCHES & OPTIONS
# ==========================================
if(IS_SUBPROJECT)
    # Turn off the test executable by default when used inside another project
    option(BUILD_EXECUTABLE "Build the main application executable" OFF)
else()
    option(BUILD_EXECUTABLE "Build the main application executable" ON)
endif()

# ==========================================
# 3. VENDORED DEPENDENCIES
# ==========================================
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# ==========================================
# 3. VENDORED DEPENDENCIES
# ==========================================
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# ONLY include fmt if a parent project hasn't already included it
if(NOT TARGET fmt)
    set(FMT_TEST OFF CACHE BOOL "" FORCE)
    set(FMT_DOC OFF CACHE BOOL "" FORCE)
    set(FMT_INSTALL OFF CACHE BOOL "" FORCE)

    add_subdirectory(${EXTERNAL_DIR}/fmt ${EXTERNAL_DIR}/fmt/build)
endif()

# ==========================================
# 4. INTERPROCEDURAL OPTIMIZATION (PROD ONLY)
# ==========================================
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error LANGUAGES CXX)

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

target_include_directories(${LIB_TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${LIB_TARGET_NAME} PUBLIC fmt::fmt)

target_compile_definitions(
    ${LIB_TARGET_NAME} 
    PUBLIC 
    RESOURCES_PATH=\"./resources/\"#"
)

if(IS_SUBPROJECT)
    target_compile_definitions(${LIB_TARGET_NAME} PUBLIC NDEBUG)
    target_compile_options(${LIB_TARGET_NAME} PRIVATE -O3)
    if(ipo_supported)
        set_target_properties(${LIB_TARGET_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
else()
    target_compile_definitions(${LIB_TARGET_NAME} PUBLIC 
        $<$<CONFIG:Debug>:DEVELOPMENT_MODE=1>
        $<$<CONFIG:Release>:NDEBUG>
    )
    target_compile_options(${LIB_TARGET_NAME} PRIVATE
        $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic>
        $<$<CONFIG:Release>:-O3>
    )
    if(ipo_supported)
        set_target_properties(${LIB_TARGET_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    endif()
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

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_options(${EXE_TARGET_NAME} PRIVATE "-static-libgcc" "-static-libstdc++" "-static")
    endif()
endif()
