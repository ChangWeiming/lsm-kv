project(lsmtree)

set(cmake_cxx_standard 17)
aux_source_directory(./config CONFIG_SRC)
aux_source_directory(./lsm LSM_SRC)
aux_source_directory(./sstable SSTABLE_SRC)
aux_source_directory(./utils UTILS_SRC)
aux_source_directory(./value VALUE)

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -Wall -DDEBUGINFO)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Wall -O3)
endif()

include_directories("./")

add_executable(lsmtree main.cpp ${CONFIG_SRC} ${LSM_SRC} ${SSTABLE_SRC} ${UTILS_SRC} ${VALUE})