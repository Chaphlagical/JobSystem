cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(JobSystem VERSION 0.0.1)
message(STATUS "[Project] ${PROJECT_NAME} ${PROJECT_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

include(Init)
Init_Project()

add_subdirectory(Source)
add_subdirectory(Test)
