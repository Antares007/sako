cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 0.1.0)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(STATIC ON CACHE BOOL "" FORCE)

add_subdirectory(extern/libgit2)
add_subdirectory(extern/OpenXLSX)

add_executable(main
  main.cpp
  git.h git.cpp
  lr.h
  overloaded.h)
target_compile_features(main PRIVATE cxx_std_17)
#target_compile_options(main PRIVATE -std=c++2a)
#target_compile_options(main PRIVATE -fconcepts)
target_link_libraries(main PRIVATE git2 OpenXLSX)

target_include_directories(main PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extern/libgit2/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extern/OpenXLSX/library/openxlsx/interfaces/c++/headers>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extern/NamedType>
  #$<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
)
