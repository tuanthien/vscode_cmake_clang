cmake_minimum_required(VERSION 3.27)

project("Example" VERSION 0.0.1 DESCRIPTION "Example description" LANGUAGES C CXX)

add_executable(Example)

# set C++ standard
set_property(TARGET Example PROPERTY C_STANDARD 17)
target_compile_features(Example INTERFACE c_std_17)

set_property(TARGET Example PROPERTY CXX_STANDARD 20)
target_compile_features(Example INTERFACE cxx_std_20)

# Download dependencies
include(_cmake/CPMPackageManager.cmake)
CPMAddPackage(
  GITHUB_REPOSITORY raysan5/raylib
  GIT_SHALLOW ON
  GIT_TAG 5.5
  OPTIONS 
    "BUILD_SHARED_LIBS OFF"
  SYSTEM YES
  EXCLUDE_FROM_ALL YES
)

# specify header/source files
target_sources(Example
  PRIVATE
    src/Example.c
)

# link dependencies
target_link_libraries(Example
  PRIVATE
    raylib_static 
)
