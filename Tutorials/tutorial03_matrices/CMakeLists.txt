cmake_minimum_required (VERSION 3.4)
project (tutorial03)

set(GLSL_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTransform.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/SingleColor.glsl
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    COMMAND $<TARGET_FILE:embed> ${GLSL_SRC}
    > ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    DEPENDS embed ${GLSL_SRC}
)

add_executable(tutorial03
    tutorial03.cpp
    _embedded_data.cpp
    ../common/shader.cpp
)

target_link_libraries(tutorial03
  ${OPENGL_LIBRARY}
  glfw
  glad
)