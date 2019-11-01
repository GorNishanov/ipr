cmake_minimum_required (VERSION 2.6)
include_directories("$(PROJECT_SOURCE_DIR)/..")



project (IprTest)
add_executable(IprTest IprTest.cpp)
target_link_libraries(IprTest ipr)

