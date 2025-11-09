# yssimplesound CMake file
cmake_minimum_required(VERSION 3.10)
project(yssimplesound LANGUAGES CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Create the yssimplesound library
add_library(yssimplesound STATIC 
    src/yssimplesound.cpp
    src/yssimplesound.h
)


# Make the src directory available for includes
target_include_directories(ysglbitmapfont PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)