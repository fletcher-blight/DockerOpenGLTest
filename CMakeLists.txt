cmake_minimum_required(VERSION 3.22)
project(gl)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm)

add_executable(gl main.cpp SimpleFragmentShader.fragmentshader SimpleVertexShader.vertexshader)
target_include_directories(gl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gl PRIVATE OpenGL::GL GLEW::GLEW glm::glm glfw)

# Runtime Shaders
file(COPY
        SimpleVertexShader.vertexshader
        SimpleFragmentShader.fragmentshader
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR})