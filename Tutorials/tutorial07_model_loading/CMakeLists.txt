cmake_minimum_required (VERSION 3.4)
project (tutorial07)

set(CMAKE_CURRENT_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

add_executable(tutorial07
    tutorial07.cpp
    ../common/shader.cpp
    ../common/texture.cpp
    ../common/controls.cpp
    ../common/objloader.cpp
)

set_target_properties(tutorial07
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tutorial07
  ${OPENGL_LIBRARY}
  glfw
  glad
  assimp
)