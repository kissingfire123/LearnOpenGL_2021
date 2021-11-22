cmake_minimum_required(VERSION 3.12)

PROJECT(SOIL LANGUAGES C)
set(SOIL_LIB_NAME SOIL)

aux_source_directory(src ALL_SOIL_SRCS)
aux_source_directory(include ALL_SOIL_HEADERS)

add_library(${SOIL_LIB_NAME} STATIC ${ALL_SOIL_SRCS}  ${ALL_SOIL_SRCS})
target_include_directories(${SOIL_LIB_NAME} PUBLIC include)
