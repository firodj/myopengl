cmake_minimum_required (VERSION 3.4)
project (quick_glfw)

add_executable(quick_glfw
    quick_glfw.cpp
    ../common/shader.cpp
)

target_link_libraries(quick_glfw
  ${OPENGL_LIBRARY}
  glfw
  glad
)