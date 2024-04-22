cmake_minimum_required(VERSION 3.0)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(glassOverFlow)

add_executable(
  ${PROJECT_NAME}
  src/main.cpp)

target_sources(${PROJECT_NAME} PRIVATE dep/glad/src/glad.c)
target_include_directories(${PROJECT_NAME} PRIVATE dep/glad/include/)

add_subdirectory(dep/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(OpenMP)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR})