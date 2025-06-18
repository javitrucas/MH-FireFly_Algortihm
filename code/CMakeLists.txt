cmake_minimum_required(VERSION 3.16)
project(MH_PractOpt C CXX)

# ----------------------------------------
# Optimizaciones de compilación
# ----------------------------------------
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Usando ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /MD)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -march=native)
endif()

# ----------------------------------------
# Incluir directorios
# ----------------------------------------
# ${CMAKE_SOURCE_DIR} apunta a ~/MH-PractOpt/code
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/..
)

# ----------------------------------------
# Ejecutables de test con CEC2017
# ----------------------------------------
add_library(cec17_test_func SHARED
    ${CMAKE_SOURCE_DIR}/cec17_test_func.c
    ${CMAKE_SOURCE_DIR}/cec17.c
)
add_executable(test ${CMAKE_SOURCE_DIR}/test.cc)
add_executable(testrandom ${CMAKE_SOURCE_DIR}/testrandom.cc)
add_executable(testsolis ${CMAKE_SOURCE_DIR}/testsolis.cc)

target_link_libraries(test PRIVATE cec17_test_func m)
target_link_libraries(testrandom PRIVATE cec17_test_func m)
target_link_libraries(testsolis PRIVATE cec17_test_func m)

# ----------------------------------------
# Aplicación Firefly
# ----------------------------------------
add_executable(firefly_app
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/firefly.cpp
)
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.16")
    set_target_properties(firefly_app PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(firefly_app PRIVATE cec17_test_func m)

target_precompile_headers(firefly_app PRIVATE
    "<firefly.h>"
)

# ----------------------------------------
# Copiado de datos de entrada
# ----------------------------------------
set(INPUT_DATA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/input_data")
set(INPUT_DATA_DEST_DIR "${CMAKE_BINARY_DIR}/input_data")
add_custom_command(TARGET firefly_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${INPUT_DATA_SOURCE_DIR}"
    "${INPUT_DATA_DEST_DIR}"
    COMMENT "Copiando carpeta input_data al directorio de construcción"
)