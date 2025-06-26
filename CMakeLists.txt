cmake_minimum_required(VERSION 3.10)
project(dbgen_project CXX)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

set(TORCH_PATH "/data5/zhaoyah/libtorch")
set(CMAKE_PREFIX_PATH "${TORCH_PATH}")

find_package(Torch REQUIRED)

set(DBGEN_SRC
    build.cpp driver.cpp bm_utils.cpp rnd.cpp print.cpp
    speed_seed.cpp text.cpp permute.cpp
)

add_executable(dbgen ${DBGEN_SRC})

set_target_properties(dbgen PROPERTIES LINKER_LANGUAGE CXX)

target_compile_options(dbgen PRIVATE -w)

target_include_directories(dbgen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(dbgen PRIVATE DBNAME=\"dss\" LINUX DB2 TPCH)

target_link_libraries(dbgen "${TORCH_LIBRARIES}")