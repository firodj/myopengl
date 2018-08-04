cmake_minimum_required (VERSION 3.4)
project (tutorial05)

set(GLSL_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/TransformVertexShader.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/TextureFragmentShader.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/uvtemplate.bmp
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    COMMAND $<TARGET_FILE:embed> ${GLSL_SRC}
    > ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    DEPENDS embed ${GLSL_SRC}
)

add_executable(tutorial05
    tutorial05.cpp
    _embedded_data.cpp
    ../common/shader.cpp
)

target_link_libraries(tutorial05
  ${OPENGL_LIBRARY}
  glfw
  glad
)