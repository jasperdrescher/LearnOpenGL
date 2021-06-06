# Minimum CMake version
cmake_minimum_required(VERSION 3.7)

# Generated project name
project(LearnOpenGL)

add_definitions(-DUNICODE)
set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Libraries")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake")
set(SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/Source")

file(GLOB_RECURSE SRC SRC_DIR
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.h"
    "${SRC_DIR}/*.hpp")

add_executable(${PROJECT_NAME} ${SRC})

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "LearnOpenGL")
set(DEPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies")

# GLFW
set(GLFW_DIR "${DEPS_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# GLM
set(GLM_DIR "${DEPS_DIR}/glm")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_DIR}")

# GLAD
set(GLAD_DIR "${DEPS_DIR}/glad")
set(GLAD_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/glad")
add_subdirectory("${GLAD_DIR}" glad)
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_BIN_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad")

# Debugging
message(STATUS "The BINARY directory is " ${PROJECT_BINARY_DIR})
message(STATUS "The SOURCE directory is " ${PROJECT_SOURCE_DIR})
