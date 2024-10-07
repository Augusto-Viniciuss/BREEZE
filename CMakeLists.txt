cmake_minimum_required(VERSION 3.17)
project(BREEZE)

set(CMAKE_CXX_STANDARD 17)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

include_directories(include
demo
shaders
)

file(GLOB SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} SDL2 SDL2_image OpenGL::GL GLEW glm::glm assimp)

add_executable(demo ./demo/main.cpp)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/demo/assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets
)

add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/shaders/ ${CMAKE_CURRENT_BINARY_DIR}/shaders
)

add_dependencies(demo copy_assets copy_shaders)

target_link_libraries(demo BREEZE)

