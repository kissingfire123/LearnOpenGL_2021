cmake_minimum_required(VERSION 3.12)

PROJECT(SOIL LANGUAGES C)


aux_source_directory(src ALL_SOIL_SRCS)
aux_source_directory(include ALL_SOIL_HEADERS)

include_directories(include)
add_library(SOIL STATIC ${ALL_SOIL_SRCS}  ${ALL_SOIL_SRCS})

