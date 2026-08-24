file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "../include/*.hpp")

add_executable(program ${SOURCES} ${HEADERS})
target_include_directories(program PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(program PRIVATE ${DYNAMIC_LINK_TARGETS})
