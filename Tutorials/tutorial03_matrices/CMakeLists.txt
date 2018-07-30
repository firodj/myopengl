cmake_minimum_required (VERSION 3.4)
project (tutorial03)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    COMMAND $<TARGET_FILE:embed>
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTransform.vertexshader
    ${CMAKE_CURRENT_SOURCE_DIR}/SingleColor.fragmentshader
    > ${CMAKE_CURRENT_SOURCE_DIR}/_embedded_data.cpp
    DEPENDS embed
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTransform.vertexshader
    ${CMAKE_CURRENT_SOURCE_DIR}/SingleColor.fragmentshader
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