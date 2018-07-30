cmake_minimum_required (VERSION 3.4)
project (tutorial02)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    COMMAND $<TARGET_FILE:embed>
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleVertexShader.vertexshader
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleFragmentShader.fragmentshader
    > ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    DEPENDS embed
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleVertexShader.vertexshader
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleFragmentShader.fragmentshader
)

add_executable(tutorial02
    tutorial02.cpp
    _embedded_data.cpp
    ../common/shader.cpp
)

target_link_libraries(tutorial02
  ${OPENGL_LIBRARY}
  glfw
  glad
)