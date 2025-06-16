cmake_minimum_required(VERSION 3.5)
PROJECT(cec17)

# KEY CHANGE 1: Incluir directorios para encontrar firefly.h y cec17.h
# . hace referencia a ~/MH-PractOpt/code/ (para cec17.h, etc.)
# .. hace referencia a ~/MH-PractOpt/ (para firefly.h)
include_directories(. ..)

ADD_EXECUTABLE(test "test.cc")
ADD_EXECUTABLE(testrandom "testrandom.cc")
ADD_EXECUTABLE(testsolis "testsolis.cc")

# KEY CHANGE 2: Define tu ejecutable. He cambiado el nombre a 'firefly_app'
# para evitar confusión con el archivo main.cpp.
# Aquí se le dice a CMake que 'firefly_app' necesita COMPILAR AMBOS:
# ../main.cpp (main.cpp en el directorio padre)
# ../firefly.cpp (firefly.cpp en el directorio padre)
ADD_EXECUTABLE(firefly_app ../main.cpp ../firefly.cpp)

ADD_LIBRARY("cec17_test_func" SHARED "cec17_test_func.c" "cec17.c")

TARGET_LINK_LIBRARIES(test "cec17_test_func")
TARGET_LINK_LIBRARIES(testrandom "cec17_test_func")
TARGET_LINK_LIBRARIES(testsolis "cec17_test_func")

# KEY CHANGE 3: Enlaza tu nuevo ejecutable 'firefly_app' con las librerías necesarias.
# Necesita la librería CEC2017 que acabas de construir ('cec17_test_func') y
# la librería matemática 'm' (para funciones como std::sqrt, std::exp, etc.).
TARGET_LINK_LIBRARIES(firefly_app cec17_test_func m)

# Define la ruta de origen de los datos de entrada
# CMAKE_SOURCE_DIR para este CMakeLists.txt es ~/MH-PractOpt/code
# Así que para llegar a ~/MH-PractOpt/input_data, necesitamos ir un nivel arriba (../)
set(INPUT_DATA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/input_data") 

# Define la ruta de destino dentro de la carpeta de construcción
# CMAKE_BINARY_DIR para este CMakeLists.txt es ~/MH-PractOpt/code/build
set(INPUT_DATA_DEST_DIR "${CMAKE_BINARY_DIR}/input_data")

# Añade un comando personalizado para copiar la carpeta después de que se construya tu aplicación
add_custom_command(TARGET firefly_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${INPUT_DATA_SOURCE_DIR}"
    "${INPUT_DATA_DEST_DIR}"
    COMMENT "Copiando carpeta input_data al directorio de construcción")