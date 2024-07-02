cmake_minimum_required(VERSION 3.10)
project(arbolb)

# Incluir el directorio actual para los archivos de encabezado
include_directories(${CMAKE_SOURCE_DIR})

# Añadir los archivos fuente
add_executable(arbolb main.cpp btree.cpp)
