cmake_minimum_required(VERSION 3.27)

project("Example" VERSION 0.0.1 DESCRIPTION "Example description")

add_executable(Example)

set_property(TARGET Example PROPERTY CXX_STANDARD 20)
set_property(TARGET Example PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE ON)
set_property(TARGET Example PROPERTY CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(Example INTERFACE cxx_std_20)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_compile_definitions(Example INTERFACE UNICODE _WIN32_WINNT=${EXAMPLE_WINAPI_VERSION})
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(Example INTERFACE /nologo /utf-8 /permissive- /Zc:throwingNew- /Zc:preprocessor /Zc:externConstexpr)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(Example INTERFACE -fvisibility=hidden -fchar8_t)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(Example INTERFACE /utf-8 /permissive- /EHsc)
  endif()
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    target_compile_options(Example INTERFACE -fvisibility=hidden)
  endif()
endif()

include(_cmake/CPMPackageManager.cmake)
if (NOT TARGET raylib)
  CPMAddPackage(
    GITHUB_REPOSITORY raysan5/raylib
    GIT_SHALLOW    ON
    GIT_TAG 5.5
    OPTIONS 
      "BUILD_SHARED_LIBS OFF"
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
  )
endif()

target_sources(
    Example
    PRIVATE
    src/Example.cpp
)

target_link_libraries(Example
  PRIVATE
  raylib_static 
)
