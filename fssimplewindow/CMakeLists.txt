# fssimplewindow CMake file
cmake_minimum_required(VERSION 3.10)
project(fssimplewindow LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(ysbitmapfont)

if(WIN32)
  # Windows: Use the Win32 implementation
  add_library(fssimplewindow STATIC src/fssimplewindow.cpp)
  target_link_libraries(fssimplewindow PUBLIC
    kernel32
    user32
    imm32
    gdi32
    opengl32
    glu32
    ysglbitmapfont
  )
  target_include_directories(fssimplewindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
else()
  # Linux: Use the GLX implementation
  add_library(fssimplewindow STATIC 
    src/glx/fsglxwrapper.cpp
    src/glx/fsglxkeymap.cpp
  )

  # Find and link OpenGL
  find_package(OpenGL REQUIRED)
  if(TARGET OpenGL::GL)
    target_link_libraries(fssimplewindow PUBLIC OpenGL::GL OpenGL::GLU)
  else()
    target_link_libraries(fssimplewindow PUBLIC ${OPENGL_LIBRARIES})
    target_include_directories(fssimplewindow PUBLIC ${OPENGL_INCLUDE_DIR})
  endif()


  # Find and link X11
  find_package(X11 REQUIRED)
  target_link_libraries(fssimplewindow PUBLIC ${X11_LIBRARIES})
  target_include_directories(fssimplewindow PUBLIC ${X11_INCLUDE_DIR})

  # Link ysglbitmapfont
  target_link_libraries(fssimplewindow PUBLIC ysglbitmapfont)
  
  # Make the src directory available for includes (so #include "../fssimplewindow.h" works)
  target_include_directories(fssimplewindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()