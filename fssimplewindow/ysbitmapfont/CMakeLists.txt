# ysglbitmapfont CMake file
cmake_minimum_required(VERSION 3.10)
project(ysglbitmapfont LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Create the ysglbitmapfont library
add_library(ysglbitmapfont STATIC 
  src/ysglfontdata.c
  src/ysglusefontbitmap.c
  src/ysglmaketextbitmap.c
  src/ysglfontdata.h
)

# Find and link OpenGL (needed for GL functions in ysglusefontbitmap.c)
find_package(OpenGL REQUIRED)
if(TARGET OpenGL::GL)
  target_link_libraries(ysglbitmapfont PUBLIC OpenGL::GL)
else()
  target_link_libraries(ysglbitmapfont PUBLIC ${OPENGL_LIBRARIES})
  target_include_directories(ysglbitmapfont PUBLIC ${OPENGL_INCLUDE_DIR})
endif()

# Make the src directory available for includes
target_include_directories(ysglbitmapfont PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)