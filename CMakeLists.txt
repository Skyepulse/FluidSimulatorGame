cmake_minimum_required(VERSION 3.0)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-O2")

project(glassOverFlow)

# Enable debugging symbols
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SOURCE
     "src/*.h"
     "src/*.cpp"
)

add_executable(
  ${PROJECT_NAME}
  ${SOURCE}
)

target_sources(${PROJECT_NAME} PRIVATE dep/glad/src/glad.c)
target_include_directories(${PROJECT_NAME} PRIVATE dep/glad/include/)

add_subdirectory(dep/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

add_subdirectory(dep/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

add_subdirectory(dep/spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

set(IMGUI_SOURCES
    dep/imgui/imgui.cpp
    dep/imgui/imgui_draw.cpp
    dep/imgui/imgui_tables.cpp
    dep/imgui/imgui_widgets.cpp
    dep/imgui/backends/imgui_impl_glfw.cpp
    dep/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE dep/imgui dep/imgui/backends)

target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

find_package(OpenMP)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS} OpenGL::GL)

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR})