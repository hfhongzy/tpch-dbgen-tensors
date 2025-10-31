cmake_minimum_required(VERSION 3.10)
project(dbgen_project CXX)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

execute_process(
  COMMAND "python3" -c
          "import torch, pathlib as p; print((p.Path(torch.__file__).resolve().parent / 'share' / 'cmake' / 'Torch'))"
  OUTPUT_VARIABLE Torch_DIR_FROM_PY
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Expose as a cache var so find_package(Torch) can use it (and user can override)
set(Torch_DIR "${Torch_DIR_FROM_PY}" CACHE PATH "Torch CMake package dir")
message(STATUS "Torch_DIR resolved to: ${Torch_DIR}")

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
