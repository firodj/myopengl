cmake_minimum_required (VERSION 3.4)
project (tutorial06)

set(GLSL_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/TransformVertexShader.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/TextureFragmentShader.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/uvtemplate.DDS
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    COMMAND $<TARGET_FILE:embed> ${GLSL_SRC}
    > ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    DEPENDS embed ${GLSL_SRC}
)

add_executable(tutorial06
    tutorial06.cpp
    _embedded_data.cpp
    ../common/shader.cpp
    ../common/texture.cpp
    ../common/controls.cpp
)

target_link_libraries(tutorial06
  ${OPENGL_LIBRARY}
  glfw
  glad
)