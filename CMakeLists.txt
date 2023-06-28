project(lsmtree)

set(cmake_cxx_standard 17)

add_executable(lsmtree main.cpp)
add_compile_options("-Wall- DDEBUGINFO -g")