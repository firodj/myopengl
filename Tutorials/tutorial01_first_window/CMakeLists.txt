cmake_minimum_required (VERSION 3.4)
project (tutorial01)

add_executable(tutorial01
    tutorial01.cpp
)

target_link_libraries(tutorial01
  ${OPENGL_LIBRARY}
  glfw
  glad
)