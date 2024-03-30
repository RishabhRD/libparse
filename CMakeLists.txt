cmake_minimum_required(VERSION 3.15)
project(libparse CXX)
include(cmake/StandardProjectSettings.cmake)
include(cmake/StaticAnalyzers.cmake)
include(cmake/CompilerWarnings.cmake)
include(package.cmake)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
set_project_warnings(project_warnings)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


option(ENABLE_TESTING "Enable Test Builds" ON)
option(ENABLE_EXAMPLES "Enable Example Builds" ON)

if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

if(ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()
