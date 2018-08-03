cmake_minimum_required (VERSION 3.4)
project (tutorial04)

set(GLSL_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/TransformVertexShader.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/ColorFragmentShader.glsl
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    COMMAND $<TARGET_FILE:embed> ${GLSL_SRC}
    > ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    DEPENDS embed ${GLSL_SRC}
)

add_executable(tutorial04
    tutorial04.cpp
    _embedded_data.cpp
    ../common/shader.cpp
)

target_link_libraries(tutorial04
  ${OPENGL_LIBRARY}
  glfw
  glad
)