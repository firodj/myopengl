cmake_minimum_required (VERSION 3.4)
project (myopengl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set (CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

### GLFW ###

add_subdirectory (Externals/glfw)

add_definitions(
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

include_directories(
  Externals/glfw/include/GLFW/
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(OPENGL_LIBRARY
    ${OPENGL_LIBRARY}
    -lGL -lGLU -lXrandr -lXext -lX11 -lrt
    ${CMAKE_DL_LIBS}
    ${GLFW_LIBRARIES}
)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(OPENGL_LIBRARY
    ${OPENGL_LIBRARY}
    ${CMAKE_DL_LIBS}
    ${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

### GLAD ###

add_subdirectory (Externals/glad)

include_directories(
  ${GLAD_BINARY_DIR}/include
)

### GLM ###

add_subdirectory (Externals/glm)

include_directories(
  Externals/glm
)

### Tools

add_executable(embed
  Tools/embed.cpp
)

include_directories(
  Tools/
)

### Tutorials

include_directories(Tutorials)

add_subdirectory(Tutorials/tutorial01_first_window)
add_subdirectory(Tutorials/tutorial02_red_triangle)
add_subdirectory(Tutorials/tutorial03_matrices)
add_subdirectory(Tutorials/tutorial04_colored_cube)
add_subdirectory(Tutorials/tutorial05_textured_cube)
add_subdirectory(Tutorials/quick_glfw)